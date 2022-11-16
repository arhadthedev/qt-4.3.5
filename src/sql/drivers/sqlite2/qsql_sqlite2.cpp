/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSql module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qsql_sqlite2.h"

#include <qcoreapplication.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qregexp.h>
#include <qsqlerror.h>
#include <qsqlfield.h>
#include <qsqlindex.h>
#include <qsqlquery.h>
#include <qstringlist.h>
#include <qvector.h>

#if !defined Q_WS_WIN32
# include <unistd.h>
#endif
#include <sqlite.h>

typedef struct sqlite_vm sqlite_vm;

Q_DECLARE_METATYPE(sqlite_vm*)
Q_DECLARE_METATYPE(sqlite*)

static QVariant::Type nameToType(const QString& typeName)
{
    QString tName = typeName.toUpper();
    if (tName.startsWith(QLatin1String("INT")))
        return QVariant::Int;
    if (tName.startsWith(QLatin1String("FLOAT")) || tName.startsWith(QLatin1String("NUMERIC")))
        return QVariant::Double;
    if (tName.startsWith(QLatin1String("BOOL")))
        return QVariant::Bool;
    // SQLite is typeless - consider everything else as string
    return QVariant::String;
}

class QSQLite2DriverPrivate
{
public:
    QSQLite2DriverPrivate();
    sqlite *access;
    bool utf8;
};

QSQLite2DriverPrivate::QSQLite2DriverPrivate() : access(0)
{
    utf8 = (qstrcmp(sqlite_encoding, "UTF-8") == 0);
}

class QSQLite2ResultPrivate
{
public:
    QSQLite2ResultPrivate(QSQLite2Result *res);
    void cleanup();
    bool fetchNext(QSqlCachedResult::ValueCache &values, int idx, bool initialFetch);
    bool isSelect();
    // initializes the recordInfo and the cache
    void init(const char **cnames, int numCols);
    void finalize();

    QSQLite2Result* q;
    sqlite *access;

    // and we have too keep our own struct for the data (sqlite works via
    // callback.
    const char *currentTail;
    sqlite_vm *currentMachine;

    uint skippedStatus: 1; // the status of the fetchNext() that's skipped
    uint skipRow: 1; // skip the next fetchNext()?
    uint utf8: 1;
    QSqlRecord rInf;
};

static const uint initial_cache_size = 128;

QSQLite2ResultPrivate::QSQLite2ResultPrivate(QSQLite2Result* res) : q(res), access(0), currentTail(0),
    currentMachine(0), skippedStatus(false), skipRow(false), utf8(false)
{
}

void QSQLite2ResultPrivate::cleanup()
{
    finalize();
    rInf.clear();
    currentTail = 0;
    currentMachine = 0;
    skippedStatus = false;
    skipRow = false;
    q->setAt(QSql::BeforeFirstRow);
    q->setActive(false);
    q->cleanup();
}

void QSQLite2ResultPrivate::finalize()
{
    if (!currentMachine)
        return;

    char* err = 0;
    int res = sqlite_finalize(currentMachine, &err);
    if (err) {
        q->setLastError(QSqlError(QCoreApplication::translate("QSQLite2Result",
                                  "Unable to fetch results"), QString::fromAscii(err),
                                  QSqlError::StatementError, res));
        sqlite_freemem(err);
    }
    currentMachine = 0;
}

// called on first fetch
void QSQLite2ResultPrivate::init(const char **cnames, int numCols)
{
    if (!cnames)
        return;

    rInf.clear();
    if (numCols <= 0)
        return;
    q->init(numCols);

    for (int i = 0; i < numCols; ++i) {
        const char* lastDot = strrchr(cnames[i], '.');
        const char* fieldName = lastDot ? lastDot + 1 : cnames[i];
        rInf.append(QSqlField(QString::fromAscii(fieldName),
                              nameToType(QString::fromAscii(cnames[i+numCols]))));
    }
}

bool QSQLite2ResultPrivate::fetchNext(QSqlCachedResult::ValueCache &values, int idx, bool initialFetch)
{
    // may be caching.
    const char **fvals;
    const char **cnames;
    int colNum;
    int res;
    int i;

    if (skipRow) {
        // already fetched
        Q_ASSERT(!initialFetch);
        skipRow = false;
        return skippedStatus;
    }
    skipRow = initialFetch;

    if (!currentMachine)
        return false;

    // keep trying while busy, wish I could implement this better.
    while ((res = sqlite_step(currentMachine, &colNum, &fvals, &cnames)) == SQLITE_BUSY) {
        // sleep instead requesting result again immidiately.
#if defined Q_WS_WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    switch(res) {
    case SQLITE_ROW:
        // check to see if should fill out columns
        if (rInf.isEmpty())
            // must be first call.
            init(cnames, colNum);
        if (!fvals)
            return false;
        if (idx < 0 && !initialFetch)
            return true;
        for (i = 0; i < colNum; ++i)
            values[i + idx] = utf8 ? QString::fromUtf8(fvals[i]) : QString::fromAscii(fvals[i]);
        return true;
    case SQLITE_DONE:
        if (rInf.isEmpty())
            // must be first call.
            init(cnames, colNum);
        q->setAt(QSql::AfterLastRow);
        return false;
    case SQLITE_ERROR:
    case SQLITE_MISUSE:
    default:
        // something wrong, don't get col info, but still return false
        finalize(); // finalize to get the error message.
        q->setAt(QSql::AfterLastRow);
        return false;
    }
    return false;
}

QSQLite2Result::QSQLite2Result(const QSQLite2Driver* db)
: QSqlCachedResult(db)
{
    d = new QSQLite2ResultPrivate(this);
    d->access = db->d->access;
    d->utf8 = db->d->utf8;
}

QSQLite2Result::~QSQLite2Result()
{
    d->cleanup();
    delete d;
}

void QSQLite2Result::virtual_hook(int id, void *data)
{
    if (id == DetachFromResultSet) {
        d->finalize();
        return;
    }
    QSqlResult::virtual_hook(id, data);
}

/*
   Execute \a query.
*/
bool QSQLite2Result::reset (const QString& query)
{
    // this is where we build a query.
    if (!driver())
        return false;
    if (!driver()-> isOpen() || driver()->isOpenError())
        return false;

    d->cleanup();

    // Um, ok.  callback based so.... pass private static function for this.
    setSelect(false);
    char *err = 0;
    int res = sqlite_compile(d->access,
                                d->utf8 ? query.toUtf8().constData()
                                        : query.toAscii().constData(),
                                &(d->currentTail),
                                &(d->currentMachine),
                                &err);
    if (res != SQLITE_OK || err) {
        setLastError(QSqlError(QCoreApplication::translate("QSQLite2Result",
                     "Unable to execute statement"), QString::fromAscii(err),
                     QSqlError::StatementError, res));
        sqlite_freemem(err);
    }
    //if (*d->currentTail != '\000' then there is more sql to eval
    if (!d->currentMachine) {
        setActive(false);
        return false;
    }
    // we have to fetch one row to find out about
    // the structure of the result set
    d->skippedStatus = d->fetchNext(cache(), 0, true);
    setSelect(!d->rInf.isEmpty());
    setActive(true);
    return true;
}

bool QSQLite2Result::gotoNext(QSqlCachedResult::ValueCache& row, int idx)
{
    return d->fetchNext(row, idx, false);
}

int QSQLite2Result::size()
{
    return -1;
}

int QSQLite2Result::numRowsAffected()
{
    return sqlite_changes(d->access);
}

QSqlRecord QSQLite2Result::record() const
{
    if (!isActive() || !isSelect())
        return QSqlRecord();
    return d->rInf;
}

QVariant QSQLite2Result::handle() const
{
    return qVariantFromValue(d->currentMachine);
}

/////////////////////////////////////////////////////////

QSQLite2Driver::QSQLite2Driver(QObject * parent)
    : QSqlDriver(parent)
{
    d = new QSQLite2DriverPrivate();
}

QSQLite2Driver::QSQLite2Driver(sqlite *connection, QObject *parent)
    : QSqlDriver(parent)
{
    d = new QSQLite2DriverPrivate();
    d->access = connection;
    setOpen(true);
    setOpenError(false);
}


QSQLite2Driver::~QSQLite2Driver()
{
    delete d;
}

bool QSQLite2Driver::hasFeature(DriverFeature f) const
{
    switch (f) {
    case Transactions:
    case SimpleLocking:
        return true;
    case Unicode:
        return d->utf8;
    default:
        return false;
    }
}

/*
   SQLite dbs have no user name, passwords, hosts or ports.
   just file names.
*/
bool QSQLite2Driver::open(const QString & db, const QString &, const QString &, const QString &, int, const QString &)
{
    if (isOpen())
        close();

    if (db.isEmpty())
        return false;

    char* err = 0;
    d->access = sqlite_open(QFile::encodeName(db), 0, &err);
    if (err) {
        setLastError(QSqlError(tr("Error to open database"), QString::fromAscii(err),
                     QSqlError::ConnectionError));
        sqlite_freemem(err);
        err = 0;
    }

    if (d->access) {
        setOpen(true);
        setOpenError(false);
        return true;
    }
    setOpenError(true);
    return false;
}

void QSQLite2Driver::close()
{
    if (isOpen()) {
        sqlite_close(d->access);
        d->access = 0;
        setOpen(false);
        setOpenError(false);
    }
}

QSqlResult *QSQLite2Driver::createResult() const
{
    return new QSQLite2Result(this);
}

bool QSQLite2Driver::beginTransaction()
{
    if (!isOpen() || isOpenError())
        return false;

    char* err;
    int res = sqlite_exec(d->access, "BEGIN", 0, this, &err);

    if (res == SQLITE_OK)
        return true;

    setLastError(QSqlError(tr("Unable to begin transaction"),
                           QString::fromAscii(err), QSqlError::TransactionError, res));
    sqlite_freemem(err);
    return false;
}

bool QSQLite2Driver::commitTransaction()
{
    if (!isOpen() || isOpenError())
        return false;

    char* err;
    int res = sqlite_exec(d->access, "COMMIT", 0, this, &err);

    if (res == SQLITE_OK)
        return true;

    setLastError(QSqlError(tr("Unable to commit transaction"),
                QString::fromAscii(err), QSqlError::TransactionError, res));
    sqlite_freemem(err);
    return false;
}

bool QSQLite2Driver::rollbackTransaction()
{
    if (!isOpen() || isOpenError())
        return false;

    char* err;
    int res = sqlite_exec(d->access, "ROLLBACK", 0, this, &err);

    if (res == SQLITE_OK)
        return true;

    setLastError(QSqlError(tr("Unable to rollback Transaction"),
                           QString::fromAscii(err), QSqlError::TransactionError, res));
    sqlite_freemem(err);
    return false;
}

QStringList QSQLite2Driver::tables(QSql::TableType type) const
{
    QStringList res;
    if (!isOpen())
        return res;

    QSqlQuery q(createResult());
    q.setForwardOnly(true);
    if ((type & QSql::Tables) && (type & QSql::Views))
        q.exec(QLatin1String("SELECT name FROM sqlite_master WHERE type='table' OR type='view'"));
    else if (type & QSql::Tables)
        q.exec(QLatin1String("SELECT name FROM sqlite_master WHERE type='table'"));
    else if (type & QSql::Views)
        q.exec(QLatin1String("SELECT name FROM sqlite_master WHERE type='view'"));

    if (q.isActive()) {
        while(q.next())
            res.append(q.value(0).toString());
    }

    if (type & QSql::SystemTables) {
        // there are no internal tables beside this one:
        res.append(QLatin1String("sqlite_master"));
    }

    return res;
}

QSqlIndex QSQLite2Driver::primaryIndex(const QString &tblname) const
{
    QSqlRecord rec(record(tblname)); // expensive :(

    if (!isOpen())
        return QSqlIndex();

    QSqlQuery q(createResult());
    q.setForwardOnly(true);
    // finrst find a UNIQUE INDEX
    q.exec(QLatin1String("PRAGMA index_list('") + tblname + QLatin1String("');"));
    QString indexname;
    while(q.next()) {
        if (q.value(2).toInt()==1) {
            indexname = q.value(1).toString();
            break;
        }
    }
    if (indexname.isEmpty())
        return QSqlIndex();

    q.exec(QLatin1String("PRAGMA index_info('") + indexname + QLatin1String("');"));

    QSqlIndex index(tblname, indexname);
    while(q.next()) {
        QString name = q.value(2).toString();
        QVariant::Type type = QVariant::Invalid;
        if (rec.contains(name))
            type = rec.field(name).type();
        index.append(QSqlField(name, type));
    }
    return index;
}

QSqlRecord QSQLite2Driver::record(const QString &tbl) const
{
    if (!isOpen())
        return QSqlRecord();

    QSqlQuery q(createResult());
    q.setForwardOnly(true);
    q.exec(QLatin1String("SELECT * FROM ") + tbl + QLatin1String(" LIMIT 1"));
    return q.record();
}

QVariant QSQLite2Driver::handle() const
{
    return qVariantFromValue(d->access);
}
