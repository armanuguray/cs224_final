#include "GLWidget.h"

#include <QTime>
#include <QTimer>
#include "CS123Algebra.h"
#include "DrawEngine.h"
#include <QMouseEvent>
#include <QWheelEvent>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::DoubleBuffer), parent), m_renderOverlay(true)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMouseTracking(true);
    //this->setAutoBufferSwap(false);
}

GLWidget::~GLWidget()
{
    delete m_timer;
    delete m_time;
    delete m_drawengine;
}

void GLWidget::initializeGL()
{
    m_drawengine = new DrawEngine(this->width(), this->height());

    // start the rendering loop
    m_time = new QTime();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(repaint()));
    m_timer->start(30.0);
}

void GLWidget::paintGL()
{
    float time = m_time->elapsed();
    m_time->restart();
    m_drawengine->drawFrame(time);

    if (m_renderOverlay) {
        this->renderOverlayText();
    }

    glFlush();
}

void GLWidget::resizeGL(int width, int height)
{
    m_drawengine->resize(width, height);
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    m_drawengine->mouse_scroll(event->delta());
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    Vector2 pos = Vector2(event->x(), event->y());
    Vector2 delta = pos - m_old;
    m_old = pos;
    if(event->buttons() & Qt::RightButton)
        m_drawengine->mouse_dragged(m_old, delta, MouseButtonRight);
    else {
        MouseButton button;
        if (event->buttons() & Qt::LeftButton)
        {
            if (event->modifiers() & Qt::ControlModifier)
                button = MouseButtonCTRLLeft;
            else button = MouseButtonLeft;

            m_drawengine->mouse_dragged(m_old, delta, button);
        }
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_old = Vector2(event->x(), event->y());
    if (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::ControlModifier)
        m_drawengine->mouse_down(m_old, MouseButtonCTRLLeft);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    bool ignore = true;

    switch (event->key()) {
    case Qt::Key_S:
        m_renderOverlay = !m_renderOverlay;
        ignore = false;
        break;

    default:
        break;
    }

    if (ignore) event->ignore();
}

void GLWidget::renderOverlayText() {
    const static int x = 10;
    const static int y = 20;
    const static int text_height = 15;
    const static QColor text_color(Qt::white);

    this->qglColor(text_color);
    this->renderText(x, y, "ESC: Exit fullscreen");
    this->renderText(x, y + text_height, "F/F11: Toggle fullscreen");
    this->renderText(x, y + 2 * text_height, "S: Toggle debug information draw");
}
