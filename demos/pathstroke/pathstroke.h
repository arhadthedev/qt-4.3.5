/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef PATHSTROKE_H
#define PATHSTROKE_H

#include "arthurwidgets.h"
#include <QtGui>

class PathStrokeRenderer : public ArthurFrame
{
    Q_OBJECT
    Q_PROPERTY(bool animation READ animation WRITE setAnimation)
    Q_PROPERTY(double penWidth READ realPenWidth WRITE setRealPenWidth)
public:
    enum PathMode { CurveMode, LineMode };

    PathStrokeRenderer(QWidget *parent);

    void paint(QPainter *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void timerEvent(QTimerEvent *e);

    QSize sizeHint() const { return QSize(500, 500); }

    bool animation() const { return m_timer.isActive(); }

    double realPenWidth() const { return m_penWidth; }
    void setRealPenWidth(double penWidth) { m_penWidth = penWidth; update(); }

public slots:
    void setPenWidth(int penWidth) { m_penWidth = penWidth / 10.0; update(); }
    void setAnimation(bool animation);

    void setFlatCap() { m_capStyle = Qt::FlatCap; update(); }
    void setSquareCap() { m_capStyle = Qt::SquareCap; update(); }
    void setRoundCap() { m_capStyle = Qt::RoundCap; update(); }

    void setBevelJoin() { m_joinStyle = Qt::BevelJoin; update(); }
    void setMiterJoin() { m_joinStyle = Qt::MiterJoin; update(); }
    void setRoundJoin() { m_joinStyle = Qt::RoundJoin; update(); }

    void setCurveMode() { m_pathMode = CurveMode; update(); }
    void setLineMode() { m_pathMode = LineMode; update(); }

    void setSolidLine() { m_penStyle = Qt::SolidLine; update(); }
    void setDashLine() { m_penStyle = Qt::DashLine; update(); }
    void setDotLine() { m_penStyle = Qt::DotLine; update(); }
    void setDashDotLine() { m_penStyle = Qt::DashDotLine; update(); }
    void setDashDotDotLine() { m_penStyle = Qt::DashDotDotLine; update(); }
    void setCustomDashLine() { m_penStyle = Qt::NoPen; update(); }

private:
    void initializePoints();
    void updatePoints();

    QBasicTimer m_timer;

    PathMode m_pathMode;

    bool m_wasAnimated;

    double m_penWidth;
    int m_pointCount;
    int m_pointSize;
    int m_activePoint;
    QVector<QPointF> m_points;
    QVector<QPointF> m_vectors;

    Qt::PenJoinStyle m_joinStyle;
    Qt::PenCapStyle m_capStyle;

    Qt::PenStyle m_penStyle;
};

class PathStrokeWidget : public QWidget
{
    Q_OBJECT
public:
    PathStrokeWidget();

private:
    PathStrokeRenderer *m_renderer;
};

#endif // PATHSTROKE_H
