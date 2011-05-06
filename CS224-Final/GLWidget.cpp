#include "GLWidget.h"

#include <QTime>
#include <QTimer>
#include "CS123Algebra.h"
#include "DrawEngine.h"
#include "Settings.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include "RigidBodyRendering.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::DoubleBuffer), parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMouseTracking(true);
    //this->setAutoBufferSwap(false);
    this->setAutoFillBackground(false);
}

GLWidget::~GLWidget()
{
    delete m_timer;
    delete m_time;
    delete m_drawengine;
}

void GLWidget::initializeGL()
{
    m_drawengine = new DrawEngine(this->context(), this->width(), this->height());

    // start the rendering loop
    m_time = new QTime();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(30.0);
}

void GLWidget::paintGL()
{
    float time = m_time->elapsed() / 1000.f;
    m_time->restart();
    m_drawengine->drawFrame(time);
    glFlush();

//    if (settings.render_overlay) {
//        this->renderOverlayText();
//    }
}

void GLWidget::resizeGL(int width, int height)
{
    m_drawengine->resize(width, height);
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    m_drawengine->zoom(event->delta());
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    Vector2 pos = Vector2(event->x(), event->y());
    Vector2 delta = pos - m_old;
    m_old = pos;

    if (event->buttons() & Qt::LeftButton) {
        if (event->modifiers() & Qt::ControlModifier) {
            // generate waves here
            m_drawengine->createWave(pos);
        } else {
            // this is a normal left mouse drag; rotate look vector
            m_drawengine->turn(delta);
        }
    } else if (event->buttons() & Qt::RightButton) {
        // this is a normal right mouse drag; translate film plane
        m_drawengine->pan(delta * 1.5f);
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_old = Vector2(event->x(), event->y());

    if (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::ControlModifier) {
        m_drawengine->createWave(m_old);
    }
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    bool ignore = true;

    switch (event->key()) {
    case Qt::Key_S:
        settings.render_overlay = !settings.render_overlay;
        ignore = false;
        break;

    default:
        break;
    }

    if (ignore) event->ignore();
}

void GLWidget::renderOverlayText() {
//    glDisable(GL_CULL_FACE);
//    const float &x = settings.overlay_text_offset.x();
//    const float &y = settings.overlay_text_offset.y();
//    const int &max_border = settings.overlay_text_max_border;

//    painter.begin(this);

//    painter.setFont(settings.overlay_text_font);
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.setPen(Qt::white);

//    QFontMetrics metrics = QFontMetrics(painter.font());
//    int border = qMax(max_border, metrics.leading());

//    QRect box = metrics.boundingRect(rect(), Qt::TextWordWrap, settings.overlay_text);

//    painter.fillRect(QRect(x, y, box.width() + 2 * border, box.height() + 2 * border), QColor(0, 0, 0, 127));
//    painter.drawText(x + border, y + border, box.width(), box.height(), Qt::TextWordWrap, settings.overlay_text);

//    painter.end();
//    glEnable(GL_CULL_FACE);

//    glRasterPos2i(10, 20);
//    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
//    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *) "text to render");
}
