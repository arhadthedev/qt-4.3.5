/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qshortcut.h"
#include "private/qwidget_p.h"

#ifndef QT_NO_SHORTCUT
#include <qevent.h>
#include <qwhatsthis.h>
#include <qmenu.h>
#include <qapplication.h>
#include <private/qapplication_p.h>
#include <private/qshortcutmap_p.h>

#define QAPP_CHECK(functionName) \
    if (!qApp) { \
        qWarning("QShortcut: Initialize QApplication before calling '" functionName "'."); \
        return; \
    }

/*!
    \class QShortcut
    \brief The QShortcut class is used to create keyboard shortcuts.

    \ingroup events
    \mainclass

    The QShortcut class provides a way of connecting keyboard
    shortcuts to Qt's \l{signals and slots} mechanism, so that
    objects can be informed when a shortcut is executed. The shortcut
    can be set up to contain all the key presses necessary to
    describe a keyboard shortcut, including the states of modifier
    keys such as \gui Shift, \gui Ctrl, and \gui Alt.

    \target mnemonic

    On certain widgets, using '&' in front of a character will
    automatically create a mnemonic (a shortcut) for that character,
    e.g. "E&xit" will create the shortcut \gui Alt+X (use '&&' to
    display an actual ampersand). The widget might consume and perform
    an action on a given shortcut. On X11 the ampersand will not be
    shown and the character will be underlined. On Windows, shortcuts
    are normally not displayed until the user presses the \gui Alt
    key, but this is a setting the user can change. On Mac such
    shortcuts do not exists, unless you explicitly call the global
    qt_set_sequence_auto_mnemonic() function; then they will appear as
    they do on X11.

    For applications that use menus, it may be more convenient to
    use the convenience functions provided in the QMenu class to
    assign keyboard shortcuts to menu items as they are created.
    Alternatively, shortcuts may be associated with other types of
    actions in the QAction class.

    The simplest way to create a shortcut for a particular widget is
    to construct the shortcut with a key sequence. For example:

    \code
        shortcut = new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")),
                                 parent);
    \endcode

    When the user types the \l{QKeySequence}{key sequence}
    for a given shortcut, the shortcut's activated() signal is
    emitted. (In the case of ambiguity, the activatedAmbiguously()
    signal is emitted.) A shortcut is "listened for" by Qt's event
    loop when the shortcut's parent widget is receiving events.

    A shortcut's key sequence can be set with setKey() and retrieved
    with key(). A shortcut can be enabled or disabled with
    setEnabled(), and can have "What's This?" help text set with
    setWhatsThis().

    \sa QShortcutEvent, QKeySequence, QAction
*/

/*!
    \fn QWidget *QShortcut::parentWidget() const

    Returns the shortcut's parent widget.
*/

/*!
    \fn void QShortcut::activated()

    This signal is emitted when the user types the shortcut's key
    sequence.

    \sa activatedAmbiguously()
*/

/*!
    \fn void QShortcut::activatedAmbiguously()

    This signal is emitted when the user types a shortcut key
    sequence that is ambiguous. For example, if one key sequence is a
    "prefix" for another and the user types these keys it isn't clear
    if they want the shorter key sequence, or if they're about to type
    more to complete the longer key sequence.

    \sa activated()
*/

/*
    \internal
    Private data accessed through d-pointer.
*/
class QShortcutPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QShortcut)
public:
    QShortcutPrivate() : sc_context(Qt::WindowShortcut), sc_enabled(true), sc_autorepeat(true), sc_id(0) {}
    QKeySequence sc_sequence;
    Qt::ShortcutContext sc_context;
    bool sc_enabled;
    bool sc_autorepeat;
    int sc_id;
    QString sc_whatsthis;
    void redoGrab(QShortcutMap &map);
};

void QShortcutPrivate::redoGrab(QShortcutMap &map)
{
    Q_Q(QShortcut);
    QWidget *parent = q->parentWidget();
    if (!parent) {
        qWarning("QShortcut: No widget parent defined");
        return;
    }

    if (sc_id)
        map.removeShortcut(sc_id, q);
    if (sc_sequence.isEmpty())
        return;
    sc_id = map.addShortcut(q, sc_sequence, sc_context);
    if (!sc_enabled)
        map.setShortcutEnabled(false, sc_id, q);
    if (!sc_autorepeat)
        map.setShortcutAutoRepeat(false, sc_id, q);
}

/*!
    Constructs a QShortcut object for the \a parent widget. Since no
    shortcut key sequence is specified, the shortcut will not emit any
    signals.

    \sa setKey()
*/
QShortcut::QShortcut(QWidget *parent)
    : QObject(*new QShortcutPrivate, parent)
{
    Q_ASSERT(parent != 0);
}

/*!
    Constructs a QShortcut object for the \a parent widget. The shortcut
    operates on its parent, listening for \l{QShortcutEvent}s that
    match the \a key sequence. Depending on the ambiguity of the
    event, the shortcut will call the \a member function, or the \a
    ambiguousMember function, if the key press was in the shortcut's
    \a context.
*/
QShortcut::QShortcut(const QKeySequence &key, QWidget *parent,
                     const char *member, const char *ambiguousMember,
                     Qt::ShortcutContext context)
    : QObject(*new QShortcutPrivate, parent)
{
    QAPP_CHECK("QShortcut");

    Q_D(QShortcut);
    Q_ASSERT(parent != 0);
    d->sc_context = context;
    d->sc_sequence = key;
    d->redoGrab(qApp->d_func()->shortcutMap);
    if (member)
        connect(this, SIGNAL(activated()), parent, member);
    if (ambiguousMember)
        connect(this, SIGNAL(activatedAmbiguously()), parent, ambiguousMember);
}

/*!
    Destroys the shortcut.
*/
QShortcut::~QShortcut()
{
    Q_D(QShortcut);
    if (qApp)
        qApp->d_func()->shortcutMap.removeShortcut(d->sc_id, this);
}

/*!
    \property QShortcut::key
    \brief the shortcut's key sequence

    This is a key sequence with an optional combination of Shift, Ctrl,
    and Alt. The key sequence may be supplied in a number of ways:

    \code
        setKey(0);                  // no signal emitted
        setKey(QKeySequence());     // no signal emitted
        setKey(0x3b1);              // Greek letter alpha
        setKey(Qt::Key_D);              // 'd', e.g. to delete
        setKey('q');                // 'q', e.g. to quit
        setKey(Qt::CTRL + Qt::Key_P);       // Ctrl+P, e.g. to print document
        setKey("Ctrl+P");           // Ctrl+P, e.g. to print document
    \endcode
*/
void QShortcut::setKey(const QKeySequence &key)
{
    Q_D(QShortcut);
    if (d->sc_sequence == key)
        return;
    QAPP_CHECK("setKey");
    d->sc_sequence = key;
    d->redoGrab(qApp->d_func()->shortcutMap);
}

QKeySequence QShortcut::key() const
{
    Q_D(const QShortcut);
    return d->sc_sequence;
}

/*!
    \property QShortcut::enabled
    \brief whether the shortcut is enabled

    An enabled shortcut emits the activated() or activatedAmbiguously()
    signal when a QShortcutEvent occurs that matches the shortcut's
    key() sequence.

    If the application is in \c WhatsThis mode the shortcut will not emit
    the signals, but will show the "What's This?" text instead.

    \sa whatsThis
*/
void QShortcut::setEnabled(bool enable)
{
    Q_D(QShortcut);
    if (d->sc_enabled == enable)
        return;
    QAPP_CHECK("setEnabled");
    d->sc_enabled = enable;
    qApp->d_func()->shortcutMap.setShortcutEnabled(enable, d->sc_id, this);
}

bool QShortcut::isEnabled() const
{
    Q_D(const QShortcut);
    return d->sc_enabled;
}

/*!
    \property QShortcut::context
    \brief the context in which the shortcut is valid

    A shortcut's context decides in which circumstances a shortcut is
    allowed to be triggered. The normal context is Qt::WindowShortcut,
    which allows the shortcut to trigger if the parent (the widget
    containing the shortcut) is a subwidget of the active top-level
    window.
*/
void QShortcut::setContext(Qt::ShortcutContext context)
{
    Q_D(QShortcut);
    if(d->sc_context == context)
        return;
    QAPP_CHECK("setContext");
    d->sc_context = context;
    d->redoGrab(qApp->d_func()->shortcutMap);
}

Qt::ShortcutContext QShortcut::context()
{
    Q_D(QShortcut);
    return d->sc_context;
}

/*!
    \property QShortcut::whatsThis
    \brief the shortcut's "What's This?" help text

    The text will be shown when the application is in "What's
    This?" mode and the user types the shortcut key() sequence.

    To set "What's This?" help on a menu item (with or without a
    shortcut key), set the help on the item's action.

    \sa QWhatsThis::inWhatsThisMode(), QAction::setWhatsThis()
*/
void QShortcut::setWhatsThis(const QString &text)
{
    Q_D(QShortcut);
    d->sc_whatsthis = text;
}

QString QShortcut::whatsThis() const
{
    Q_D(const QShortcut);
    return d->sc_whatsthis;
}

/*!
    \property QShortcut::autoRepeat
    \brief whether the shortcut can auto repeat
    \since 4.2

    If true, the shortcut will auto repeat when the keyboard shortcut
    combination is held down, provided that keyboard auto repeat is
    enabled on the system.
    The default value is true.
*/
void QShortcut::setAutoRepeat(bool on)
{
    Q_D(QShortcut);
    if (d->sc_autorepeat == on)
        return;
    QAPP_CHECK("setAutoRepeat");
    d->sc_autorepeat = on;
    qApp->d_func()->shortcutMap.setShortcutAutoRepeat(on, d->sc_id, this);
}

bool QShortcut::autoRepeat() const
{
    Q_D(const QShortcut);
    return d->sc_autorepeat;
}

/*!
    Returns the shortcut's ID.

    \sa QShortcutEvent::shortcutId()
*/
int QShortcut::id() const
{
    Q_D(const QShortcut);
    return d->sc_id;
}

/*!
    \internal
*/
bool QShortcut::event(QEvent *e)
{
    Q_D(QShortcut);
    bool handled = false;
    if (d->sc_enabled && e->type() == QEvent::Shortcut) {
        QShortcutEvent *se = static_cast<QShortcutEvent *>(e);
        if (se->shortcutId() == d->sc_id && se->key() == d->sc_sequence){
#ifndef QT_NO_WHATSTHIS
            if (QWhatsThis::inWhatsThisMode()) {
                QWhatsThis::showText(QCursor::pos(), d->sc_whatsthis);
                handled = true;
            } else
#endif
            if (se->isAmbiguous())
                emit activatedAmbiguously();
            else
                emit activated();
            handled = true;
        }
    }
    return handled;
}
#endif // QT_NO_SHORTCUT
