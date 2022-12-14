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

#include "pathstroke.h"
#include "arthurstyle.h"
#include "arthurwidgets.h"

extern void draw_round_rect(QPainter *p, const QRect &bounds, int radius);

PathStrokeWidget::PathStrokeWidget()
{
    setWindowTitle(tr("Path Stroking"));

    // Setting up palette.
    QPalette pal = palette();
//     pal.setBrush(QPalette::Background, Qt::white);
//     pal.setBrush(QPalette::Foreground, QColor("aquamarine"));
//     pal.setBrush(QPalette::Background, QPixmap("background.png"));
    setPalette(pal);

    // Widget construction and property setting
    m_renderer = new PathStrokeRenderer(this);

    QGroupBox *mainGroup = new QGroupBox(this);
//     QWidget *mainGroup = new QWidget(this);
    mainGroup->setFixedWidth(180);
    mainGroup->setTitle("Path Stroking");

    QGroupBox *capGroup = new QGroupBox(mainGroup);
    capGroup->setAttribute(Qt::WA_ContentsPropagated);
    QRadioButton *flatCap = new QRadioButton(capGroup);
    QRadioButton *squareCap = new QRadioButton(capGroup);
    QRadioButton *roundCap = new QRadioButton(capGroup);
    capGroup->setTitle("Cap Style");
    flatCap->setText("Flat Cap");
    squareCap->setText("Square Cap");
    roundCap->setText("Round Cap");

    QGroupBox *joinGroup = new QGroupBox(mainGroup);
    joinGroup->setAttribute(Qt::WA_ContentsPropagated);
    QRadioButton *bevelJoin = new QRadioButton(joinGroup);
    QRadioButton *miterJoin = new QRadioButton(joinGroup);
    QRadioButton *roundJoin = new QRadioButton(joinGroup);
    joinGroup->setTitle("Join Style");
    bevelJoin->setText("Bevel Join");
    miterJoin->setText("Miter Join");
    roundJoin->setText("Round Join");

    QGroupBox *styleGroup = new QGroupBox(mainGroup);
    styleGroup->setAttribute(Qt::WA_ContentsPropagated);
    QRadioButton *solidLine = new QRadioButton(styleGroup);
    QRadioButton *dashLine = new QRadioButton(styleGroup);
    QRadioButton *dotLine = new QRadioButton(styleGroup);
    QRadioButton *dashDotLine = new QRadioButton(styleGroup);
    QRadioButton *dashDotDotLine = new QRadioButton(styleGroup);
    QRadioButton *customDashLine = new QRadioButton(styleGroup);
    styleGroup->setTitle("Pen Style");
#if 0
    solidLine->setText("Solid Line");
    dashLine->setText("Dash Line");
    dotLine->setText("Dot Line");
    dashDotLine->setText("Dash Dot Line");
    dashDotDotLine->setText("Dash Dot Dot Line");
#else
    QPixmap line_solid(":res/images/line_solid.png");
    solidLine->setIcon(line_solid);
    solidLine->setIconSize(line_solid.size());
    QPixmap line_dashed(":res/images/line_dashed.png");
    dashLine->setIcon(line_dashed);
    dashLine->setIconSize(line_dashed.size());
    QPixmap line_dotted(":res/images/line_dotted.png");
    dotLine->setIcon(line_dotted);
    dotLine->setIconSize(line_dotted.size());
    QPixmap line_dash_dot(":res/images/line_dash_dot.png");
    dashDotLine->setIcon(line_dash_dot);
    dashDotLine->setIconSize(line_dash_dot.size());
    QPixmap line_dash_dot_dot(":res/images/line_dash_dot_dot.png");
    dashDotDotLine->setIcon(line_dash_dot_dot);
    dashDotDotLine->setIconSize(line_dash_dot_dot.size());
    customDashLine->setText("Custom Style");

    int fixedHeight = bevelJoin->sizeHint().height();
    solidLine->setFixedHeight(fixedHeight);
    dashLine->setFixedHeight(fixedHeight);
    dotLine->setFixedHeight(fixedHeight);
    dashDotLine->setFixedHeight(fixedHeight);
    dashDotDotLine->setFixedHeight(fixedHeight);
#endif

    QGroupBox *pathModeGroup = new QGroupBox(mainGroup);
    pathModeGroup->setAttribute(Qt::WA_ContentsPropagated);
    QRadioButton *curveMode = new QRadioButton(pathModeGroup);
    QRadioButton *lineMode = new QRadioButton(pathModeGroup);
    pathModeGroup->setTitle("Path composed of");
    curveMode->setText("Curves");
    lineMode->setText("Lines");

    QGroupBox *penWidthGroup = new QGroupBox(mainGroup);
    penWidthGroup->setAttribute(Qt::WA_ContentsPropagated);
    QSlider *penWidth = new QSlider(Qt::Horizontal, penWidthGroup);
    penWidth->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    penWidthGroup->setTitle("Pen Width");
    penWidth->setRange(0, 500);

#if 0
    QCheckBox *animated = new QCheckBox(mainGroup);
    animated->setText("Animated");
#else
    QPushButton *animated = new QPushButton(mainGroup);
    animated->setText("Animate");
    animated->setCheckable(true);
#endif

    QPushButton *showSourceButton = new QPushButton(mainGroup);
    showSourceButton->setText("Show Source");
#ifdef QT_OPENGL_SUPPORT
    QPushButton *enableOpenGLButton = new QPushButton(mainGroup);
    enableOpenGLButton->setText("Use OpenGL");
    enableOpenGLButton->setCheckable(true);
    enableOpenGLButton->setChecked(m_renderer->usesOpenGL());
    if (!QGLFormat::hasOpenGL())
        enableOpenGLButton->hide();
#endif
    QPushButton *whatsThisButton = new QPushButton(mainGroup);
    whatsThisButton->setText("What's This?");
    whatsThisButton->setCheckable(true);

    // Layouting
    QHBoxLayout *viewLayout = new QHBoxLayout(this);
    viewLayout->addWidget(m_renderer);
    viewLayout->addWidget(mainGroup);

    QVBoxLayout *mainGroupLayout = new QVBoxLayout(mainGroup);
    mainGroupLayout->setMargin(3);
    mainGroupLayout->addWidget(capGroup);
    mainGroupLayout->addWidget(joinGroup);
    mainGroupLayout->addWidget(styleGroup);
    mainGroupLayout->addWidget(penWidthGroup);
    mainGroupLayout->addWidget(pathModeGroup);
    mainGroupLayout->addWidget(animated);
    mainGroupLayout->addStretch(1);
    mainGroupLayout->addWidget(showSourceButton);
#ifdef QT_OPENGL_SUPPORT
    mainGroupLayout->addWidget(enableOpenGLButton);
#endif
    mainGroupLayout->addWidget(whatsThisButton);

    QVBoxLayout *capGroupLayout = new QVBoxLayout(capGroup);
    capGroupLayout->addWidget(flatCap);
    capGroupLayout->addWidget(squareCap);
    capGroupLayout->addWidget(roundCap);

    QVBoxLayout *joinGroupLayout = new QVBoxLayout(joinGroup);
    joinGroupLayout->addWidget(bevelJoin);
    joinGroupLayout->addWidget(miterJoin);
    joinGroupLayout->addWidget(roundJoin);

    QVBoxLayout *styleGroupLayout = new QVBoxLayout(styleGroup);
    styleGroupLayout->addWidget(solidLine);
    styleGroupLayout->addWidget(dashLine);
    styleGroupLayout->addWidget(dotLine);
    styleGroupLayout->addWidget(dashDotLine);
    styleGroupLayout->addWidget(dashDotDotLine);
    styleGroupLayout->addWidget(customDashLine);

    QVBoxLayout *pathModeGroupLayout = new QVBoxLayout(pathModeGroup);
    pathModeGroupLayout->addWidget(curveMode);
    pathModeGroupLayout->addWidget(lineMode);

    QVBoxLayout *penWidthLayout = new QVBoxLayout(penWidthGroup);
    penWidthLayout->addWidget(penWidth);

    // Set up connections
    connect(penWidth, SIGNAL(valueChanged(int)),
            m_renderer, SLOT(setPenWidth(int)));
    connect(animated, SIGNAL(toggled(bool)),
            m_renderer, SLOT(setAnimation(bool)));

    connect(flatCap, SIGNAL(clicked()), m_renderer, SLOT(setFlatCap()));
    connect(squareCap, SIGNAL(clicked()), m_renderer, SLOT(setSquareCap()));
    connect(roundCap, SIGNAL(clicked()), m_renderer, SLOT(setRoundCap()));

    connect(bevelJoin, SIGNAL(clicked()), m_renderer, SLOT(setBevelJoin()));
    connect(miterJoin, SIGNAL(clicked()), m_renderer, SLOT(setMiterJoin()));
    connect(roundJoin, SIGNAL(clicked()), m_renderer, SLOT(setRoundJoin()));

    connect(curveMode, SIGNAL(clicked()), m_renderer, SLOT(setCurveMode()));
    connect(lineMode, SIGNAL(clicked()), m_renderer, SLOT(setLineMode()));

    connect(solidLine, SIGNAL(clicked()), m_renderer, SLOT(setSolidLine()));
    connect(dashLine, SIGNAL(clicked()), m_renderer, SLOT(setDashLine()));
    connect(dotLine, SIGNAL(clicked()), m_renderer, SLOT(setDotLine()));
    connect(dashDotLine, SIGNAL(clicked()), m_renderer, SLOT(setDashDotLine()));
    connect(dashDotDotLine, SIGNAL(clicked()), m_renderer, SLOT(setDashDotDotLine()));
    connect(customDashLine, SIGNAL(clicked()), m_renderer, SLOT(setCustomDashLine()));

    connect(showSourceButton, SIGNAL(clicked()), m_renderer, SLOT(showSource()));
#ifdef QT_OPENGL_SUPPORT
    connect(enableOpenGLButton, SIGNAL(clicked(bool)), m_renderer, SLOT(enableOpenGL(bool)));
#endif
    connect(whatsThisButton, SIGNAL(clicked(bool)), m_renderer, SLOT(setDescriptionEnabled(bool)));
    connect(m_renderer, SIGNAL(descriptionEnabledChanged(bool)),
            whatsThisButton, SLOT(setChecked(bool)));

    // Set the defaults
    animated->setChecked(true);
    flatCap->setChecked(true);
    bevelJoin->setChecked(true);
    penWidth->setValue(50);
    curveMode->setChecked(true);
    solidLine->setChecked(true);

    m_renderer->loadSourceFile(":res/pathstroke.cpp");
    m_renderer->loadDescription(":res/pathstroke.html");
}


PathStrokeRenderer::PathStrokeRenderer(QWidget *parent)
    : ArthurFrame(parent)
{
    m_pointSize = 10;
    m_activePoint = -1;
    m_capStyle = Qt::FlatCap;
    m_joinStyle = Qt::BevelJoin;
    m_pathMode = CurveMode;
    m_penWidth = 1;
    m_penStyle = Qt::SolidLine;
    m_wasAnimated = true;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void PathStrokeRenderer::paint(QPainter *painter)
{
    if (m_points.isEmpty())
        initializePoints();

    painter->setRenderHint(QPainter::Antialiasing);

    QPalette pal = palette();
    painter->setPen(Qt::NoPen);

    // Construct the path
    QPainterPath path;
    path.moveTo(m_points.at(0));

    if (m_pathMode == LineMode) {
        for (int i=1; i<m_points.size(); ++i) {
            path.lineTo(m_points.at(i));
        }
    } else {
        int i=1;
        while (i + 2 < m_points.size()) {
            path.cubicTo(m_points.at(i), m_points.at(i+1), m_points.at(i+2));
            i += 3;
        }
        while (i < m_points.size()) {
            path.lineTo(m_points.at(i));
            ++i;
        }
    }

    // Draw the path
    {
        QColor lg = Qt::red;

        // The "custom" pen
        if (m_penStyle == Qt::NoPen) {
            QPainterPathStroker stroker;
            stroker.setWidth(m_penWidth);
            stroker.setJoinStyle(m_joinStyle);
            stroker.setCapStyle(m_capStyle);

            QVector<qreal> dashes;
            qreal space = 4;
            dashes << 1 << space
                   << 3 << space
                   << 9 << space
                   << 27 << space
                   << 9 << space
                   << 3 << space;
            stroker.setDashPattern(dashes);
            QPainterPath stroke = stroker.createStroke(path);
            painter->fillPath(stroke, lg);

        } else {
            QPen pen(lg, m_penWidth, m_penStyle, m_capStyle, m_joinStyle);
            painter->strokePath(path, pen);
        }
    }

    if (1) {
        // Draw the control points
        painter->setPen(QColor(50, 100, 120, 200));
        painter->setBrush(QColor(200, 200, 210, 120));
        for (int i=0; i<m_points.size(); ++i) {
            QPointF pos = m_points.at(i);
            painter->drawEllipse(QRectF(pos.x() - m_pointSize,
                                       pos.y() - m_pointSize,
                                       m_pointSize*2, m_pointSize*2));
        }
        painter->setPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawPolyline(m_points);
    }

}

void PathStrokeRenderer::initializePoints()
{
    const int count = 7;
    m_points.clear();
    m_vectors.clear();

    QMatrix m;
    double rot = 360 / count;
    QPointF center(width() / 2, height() / 2);
    QMatrix vm;
    vm.shear(2, -1);
    vm.scale(3, 3);

    for (int i=0; i<count; ++i) {
        m_vectors << QPointF(.1f, .25f) * (m * vm);
        m_points << QPointF(0, 100) * m + center;
        m.rotate(rot);
    }
}

void PathStrokeRenderer::updatePoints()
{
    double pad = 10;
    double left = pad;
    double right = width() - pad;
    double top = pad;
    double bottom = height() - pad;

    Q_ASSERT(m_points.size() == m_vectors.size());
    for (int i=0; i<m_points.size(); ++i) {

        if (i == m_activePoint)
            continue;

        QPointF pos = m_points.at(i);
        QPointF vec = m_vectors.at(i);
        pos += vec;
        if (pos.x() < left || pos.x() > right) {
            vec.setX(-vec.x());
            pos.setX(pos.x() < left ? left : right);
        } if (pos.y() < top || pos.y() > bottom) {
            vec.setY(-vec.y());
            pos.setY(pos.y() < top ? top : bottom);
        }
        m_points[i] = pos;
        m_vectors[i] = vec;
    }
    update();
}

void PathStrokeRenderer::mousePressEvent(QMouseEvent *e)
{
    setDescriptionEnabled(false);
    m_activePoint = -1;
    qreal distance = -1;
    for (int i=0; i<m_points.size(); ++i) {
        qreal d = QLineF(e->pos(), m_points.at(i)).length();
        if ((distance < 0 && d < 8 * m_pointSize) || d < distance) {
            distance = d;
            m_activePoint = i;
        }
    }

    if (m_activePoint != -1) {
        m_wasAnimated = m_timer.isActive();
        setAnimation(false);
        mouseMoveEvent(e);
    }
}

void PathStrokeRenderer::mouseMoveEvent(QMouseEvent *e)
{
    if (m_activePoint >= 0 && m_activePoint < m_points.size()) {
        m_points[m_activePoint] = e->pos();
        update();
    }
}

void PathStrokeRenderer::mouseReleaseEvent(QMouseEvent *)
{
    m_activePoint = -1;
    setAnimation(m_wasAnimated);
}

void PathStrokeRenderer::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_timer.timerId()) {
        updatePoints();
        QApplication::syncX();
    } // else if (e->timerId() == m_fpsTimer.timerId()) {
//         emit frameRate(m_frameCount);
//         m_frameCount = 0;
//     }
}

void PathStrokeRenderer::setAnimation(bool animation)
{
    m_timer.stop();
//     m_fpsTimer.stop();

    if (animation) {
        m_timer.start(25, this);
//         m_fpsTimer.start(1000, this);
//         m_frameCount = 0;
    }
}
