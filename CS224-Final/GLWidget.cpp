#include "GLWidget.h"

#ifdef __APPLE__
    #include <QtOpenGL/qgl.h>
#else
    #include <qgl.h>
#endif

#include <QTimer>

GLWidget::GLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::DoubleBuffer), parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMouseTracking(true);
    //this->setAutoBufferSwap(false);
}

GLWidget::~GLWidget()
{
    delete m_timer;
}

void GLWidget::initializeGL()
{
    glViewport(0, 0, this->width(), this->height());
    logln("Initializing GLWidget");

    // start the rendering loop
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(repaint()));
    m_timer->start(30.0);
}

void GLWidget::paintGL()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glRotatef(0.5, 0.0, 0.0, 1.0);
    glBegin(GL_TRIANGLES);
    {
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(-0.5, -0.5, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.5, -0.5, 0.0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.5, 0.0);
    }
    glEnd();
}

void GLWidget::resizeGL(int width, int height)
{

}
