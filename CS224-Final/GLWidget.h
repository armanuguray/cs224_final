/**
 *
 * @author auguray
 * @paragraph QGLWidget subclass for the OpenGL context to draw into.
 *
 */
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "CS123Algebra.h"

#ifdef __APPLE__
#include <QtOpenGL/QGLWidget>
#else
#include <QGLWidget>
#endif

class QTime;
class QTimer;
class DrawEngine;

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

protected:
    // QGLWidget overrides
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    // mouse events
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    // keyboard event
    virtual void keyPressEvent(QKeyEvent *event);

    // the overlay text
    void renderOverlayText();

private:
    QTime *m_time;
    QTimer *m_timer;
    DrawEngine *m_drawengine;

    QPainter painter;

    Vector2 m_old; // previous mouse location for mouse interaction
};

#endif // GLWIDGET_H
