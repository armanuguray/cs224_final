/**
 *
 * @author auguray
 * @paragraph QGLWidget subclass for the OpenGL context to draw into.
 *
 */
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "OpenGLInclude.h"
#include "CS123Algebra.h"

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

private:
    QTimer *m_timer;
    DrawEngine *m_drawengine;

    Vector2 m_old; // previous mouse location for mouse interaction
};

#endif // GLWIDGET_H
