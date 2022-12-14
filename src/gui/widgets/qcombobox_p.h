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

#ifndef QCOMBOBOX_P_H
#define QCOMBOBOX_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qcombobox.h"

#ifndef QT_NO_COMBOBOX
#include "QtGui/qabstractslider.h"
#include "QtGui/qapplication.h"
#include "qitemdelegate.h"
#include "QtGui/qlineedit.h"
#include "QtGui/qlistview.h"
#include "QtGui/qpainter.h"
#include "QtGui/qstyle.h"
#include "QtGui/qstyleoption.h"
#include "QtCore/qhash.h"
#include "QtCore/qpair.h"
#include "QtCore/qtimer.h"
#include "private/qwidget_p.h"
#include "QtCore/qpointer.h"
#include "QtGui/qcompleter.h"
#include "QtGui/qevent.h"
#include "QtCore/qdebug.h"

#include <limits.h>

class QComboBoxListView : public QListView
{
    Q_OBJECT
public:
        QComboBoxListView(QComboBox *cmb = 0) : combo(cmb){};

protected:
    void resizeEvent(QResizeEvent *event)
    {
        resizeContents(viewport()->width(), contentsSize().height());
        QListView::resizeEvent(event);
    }

    QStyleOptionViewItem viewOptions() const
    {
        QStyleOptionViewItem option = QListView::viewOptions();
        option.showDecorationSelected = true;
        if (combo)
            option.font = combo->font();
        return option;
    }

    void paintEvent(QPaintEvent *e)
    {
        if (combo) {
            QStyleOptionComboBox opt;
            opt.initFrom(combo);
            opt.editable = combo->isEditable();
            if (combo->style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, combo)) {
                //we paint the empty menu area to avoid having blank space that can happen when scrolling
                QStyleOptionMenuItem menuOpt;
                menuOpt.initFrom(this);
                menuOpt.palette = palette();
                menuOpt.state = QStyle::State_None;
                menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
                menuOpt.menuRect = e->rect();
                menuOpt.maxIconWidth = 0;
                menuOpt.tabWidth = 0;
                QPainter p(viewport());
                combo->style()->drawControl(QStyle::CE_MenuEmptyArea, &menuOpt, &p, this);
            }
        }
        QListView::paintEvent(e);
    }

private:
    QComboBox *combo;
};


class QStandardItemModel;

class QComboBoxPrivateScroller : public QWidget
{
    Q_OBJECT

public:
    QComboBoxPrivateScroller(QAbstractSlider::SliderAction action, QWidget *parent)
        : QWidget(parent), sliderAction(action)
    {
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        setAttribute(Qt::WA_NoMousePropagation);
    }
    QSize sizeHint() const {
        return QSize(20, style()->pixelMetric(QStyle::PM_MenuScrollerHeight));
    }

protected:
    inline void stopTimer() {
        timer.stop();
    }

    inline void startTimer() {
        timer.start(100, this);
        fast = false;
    }

    void enterEvent(QEvent *) {
        startTimer();
    }

    void leaveEvent(QEvent *) {
        stopTimer();
    }
    void timerEvent(QTimerEvent *e) {
        if (e->timerId() == timer.timerId()) {
            emit doScroll(sliderAction);
            if (fast) {
                emit doScroll(sliderAction);
                emit doScroll(sliderAction);
            }
        }
    }
    void hideEvent(QHideEvent *) {
        stopTimer();
    }

    void mouseMoveEvent(QMouseEvent *e)
    {
        // Enable fast scrolling if the cursor is directly above or below the popup.
        const int mouseX = e->pos().x();
        const int mouseY = e->pos().y();
        const bool horizontallyInside = pos().x() < mouseX && mouseX < rect().right() + 1;
        const bool verticallyOutside = (sliderAction == QAbstractSlider::SliderSingleStepAdd) ?
                                        rect().bottom() + 1 < mouseY : mouseY < pos().y();

        fast = horizontallyInside && verticallyOutside;
    }

    void paintEvent(QPaintEvent *) {
        QPainter p(this);
        QStyleOptionMenuItem menuOpt;
        menuOpt.init(this);
        menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
        menuOpt.menuRect = rect();
        menuOpt.maxIconWidth = 0;
        menuOpt.tabWidth = 0;
        menuOpt.menuItemType = QStyleOptionMenuItem::Scroller;
        if (sliderAction == QAbstractSlider::SliderSingleStepAdd)
            menuOpt.state |= QStyle::State_DownArrow;
        p.eraseRect(rect());
        style()->drawControl(QStyle::CE_MenuScroller, &menuOpt, &p);
    }

Q_SIGNALS:
    void doScroll(int action);

private:
    QAbstractSlider::SliderAction sliderAction;
    QBasicTimer timer;
    bool fast;
};

class QComboBoxPrivateContainer : public QFrame
{
    Q_OBJECT

public:
    QComboBoxPrivateContainer(QAbstractItemView *itemView, QComboBox *parent);
    QAbstractItemView *itemView() const;
    void setItemView(QAbstractItemView *itemView);
    int spacing() const;
    void updateTopBottomMargin();

    QTimer blockMouseReleaseTimer;
    QBasicTimer adjustSizeTimer;
    QPoint initialClickPosition;

public Q_SLOTS:
    void scrollItemView(int action);
    void updateScrollers();
    void setCurrentIndex(const QModelIndex &index);
    void viewDestroyed();

protected:
    void changeEvent(QEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void timerEvent(QTimerEvent *timerEvent);
    void leaveEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    QStyleOptionComboBox comboStyleOption() const;

Q_SIGNALS:
    void itemSelected(const QModelIndex &);
    void resetButton();

private:
    QComboBox *combo;
    QAbstractItemView *view;
    QComboBoxPrivateScroller *top;
    QComboBoxPrivateScroller *bottom;
};

class QComboMenuDelegate : public QAbstractItemDelegate
{
public:
    QComboMenuDelegate(QObject *parent, QComboBox *cmb) : QAbstractItemDelegate(parent), mCombo(cmb), pal(QApplication::palette("QMenu")) {}

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const {
        QStyleOptionMenuItem opt = getStyleOption(option, index);
        painter->eraseRect(option.rect);
        mCombo->style()->drawControl(QStyle::CE_MenuItem, &opt, painter, mCombo);
    }
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const {
        QStyleOptionMenuItem opt = getStyleOption(option, index);
        QVariant value = index.model()->data(index, Qt::FontRole);
        QFont fnt = value.isValid() ? qvariant_cast<QFont>(value) : option.font;
        return mCombo->style()->sizeFromContents(
            QStyle::CT_MenuItem, &opt, option.rect.size(), mCombo);
    }

private:
    QStyleOptionMenuItem getStyleOption(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const;
    QComboBox *mCombo;
    QPalette pal;
};

class QComboBoxPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QComboBox)
public:
    QComboBoxPrivate();
    ~QComboBoxPrivate() {}
    void init();
    QComboBoxPrivateContainer* viewContainer();
    void updateLineEditGeometry();
    void _q_returnPressed();
    void _q_complete();
    void _q_itemSelected(const QModelIndex &item);
    bool contains(const QString &text, int role);
    void emitActivated(const QModelIndex&);
    void _q_emitHighlighted(const QModelIndex&);
    void _q_emitCurrentIndexChanged(const QModelIndex &index);
    void _q_modelDestroyed();
    void _q_modelReset();
#ifdef QT_KEYPAD_NAVIGATION
    void _q_completerActivated();
#endif
    void _q_resetButton();
    void _q_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void _q_rowsAboutToBeInserted(const QModelIndex & parent, int start, int end);
    void _q_rowsInserted(const QModelIndex & parent, int start, int end);
    void _q_rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
    void _q_rowsRemoved(const QModelIndex & parent, int start, int end);
    void updateArrow(QStyle::StateFlag state);
    bool updateHoverControl(const QPoint &pos);
    QRect popupGeometry(int screen = -1) const;
    QStyle::SubControl newHoverControl(const QPoint &pos);
    int computeWidthHint() const;
    QSize recomputeSizeHint(QSize &sh) const;
    void adjustComboBoxSize();
    QString itemText(const QModelIndex &index) const;
    int itemRole() const;
    void updateLayoutDirection();

    QAbstractItemModel *model;
    QLineEdit *lineEdit;
    QComboBoxPrivateContainer *container;
    QComboBox::InsertPolicy insertPolicy;
    QComboBox::SizeAdjustPolicy sizeAdjustPolicy;
    int minimumContentsLength;
    QSize iconSize;
    uint shownOnce : 1;
    uint autoCompletion : 1;
    uint duplicatesEnabled : 1;
    uint frame : 1;
    uint padding : 26;
    int maxVisibleItems;
    int maxCount;
    int modelColumn;
    bool inserting;
    mutable QSize minimumSizeHint;
    mutable QSize sizeHint;
    QStyle::StateFlag arrowState;
    QStyle::SubControl hoverControl;
    QRect hoverRect;
    QPersistentModelIndex currentIndex;
    QPersistentModelIndex root;
    Qt::CaseSensitivity autoCompletionCaseSensitivity;
    int indexBeforeChange;
#ifndef QT_NO_COMPLETER
    QPointer<QCompleter> completer;
#endif
};

#endif // QT_NO_COMBOBOX

#endif // QCOMBOBOX_P_H
