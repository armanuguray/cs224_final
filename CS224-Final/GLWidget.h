/**
 *
 * @author auguray
 * @paragraph QGLWidget subclass for the OpenGL context to draw into.
 *
 */
#ifndef GLWIDGET_H
#define GLWIDGET_H

#ifdef __APPLE__
    #include <QtOpenGL/QGLWidget>
#else
    #include <QGLWidget>
#endif

class QTimer;

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

private:
    QTimer *m_timer;
};

#endif // GLWIDGET_H
