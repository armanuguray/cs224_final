#include "GLWidget.h"

#include <QTimer>
#include "CS123Algebra.h"
#include "DrawEngine.h"
#include <QMouseEvent>
#include <QWheelEvent>

GLWidget::GLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::DoubleBuffer), parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMouseTracking(true);
    //this->setAutoBufferSwap(false);
}

GLWidget::~GLWidget()
{
    delete m_timer;
    delete m_drawengine;
}

void GLWidget::initializeGL()
{
    m_drawengine = new DrawEngine(this->width(), this->height());

    // start the rendering loop
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(repaint()));
    m_timer->start(30.0);
}

void GLWidget::paintGL()
{
    m_drawengine->drawFrame();
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
        m_drawengine->mouse_dragged(delta, MouseButtonRight);
    else if (event->buttons() & Qt::LeftButton)
        m_drawengine->mouse_dragged(delta, MouseButtonLeft);

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_old = Vector2(event->x(), event->y());
}
