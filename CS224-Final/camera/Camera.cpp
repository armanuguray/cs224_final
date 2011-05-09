#include "Camera.h"

#ifdef __APPLE__
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif


Camera::Camera(int width, int height)
{
    m_width = width;
    m_height = height;

    m_eye.init(15, 15, 15, 1);
    m_look = -m_eye, m_look.w = 0, m_look.normalize();
    m_up.x = 0.0f, m_up.y = 1.0f, m_up.z = 0.0f, m_up.w = 0;
    m_fovy = 45.0f, m_near = 0.1f, m_far = 10000.f;

    loadMatrices();
}

void Camera::loadMatrices()
{
    glViewport(0, 0, m_width, m_height);
    REAL ratio = m_width / static_cast<REAL>(m_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_fovy, ratio, m_near, m_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Vector4 center = m_eye + m_look;
    gluLookAt(m_eye.x, m_eye.y, m_eye.z,
              center.x, center.y, center.z,
              m_up.x, m_up.y, m_up.z);

    glGetDoublev(GL_MODELVIEW_MATRIX, m_modelview_matrix);
    glGetDoublev(GL_PROJECTION_MATRIX, m_projection_matrix);

    m_modelview = Matrix4x4((REAL*)m_modelview_matrix).getTranspose();
    m_modelview_inv = m_modelview.getInverse();

    m_projection = Matrix4x4((REAL*)m_projection_matrix).getTranspose();
    m_projection_inv = m_projection.getInverse();
}

void Camera::lookVectorRotate(const Vector2 &delta)
{
    Vector4 w = getW();
    REAL angleX = asinf(-w.y) - delta.y * 0.0025;
    REAL angleY = atan2f(-w.z, -w.x) + delta.x * 0.0025;
    angleX = MAX(-M_PI / 2 + 0.001, MIN(M_PI / 2 - 0.001, (double)angleX));
    m_look = Vector4(cosf(angleY) * cosf(angleX), sinf(angleX), sinf(angleY) * cosf(angleX), 0).getNormalized();
    loadMatrices();
}

void Camera::lookVectorTranslate(REAL delta)
{
    m_eye += getW() * (delta * -0.005);
    loadMatrices();
}

void Camera::filmPlaneTranslate(const Vector2 &delta)
{
    m_eye += (getU() * delta.x - getV() * delta.y) * 0.01;
    loadMatrices();
}

void Camera::resize(int width, int height)
{
    m_width = width;
    m_height = height;
    loadMatrices();
}

void Camera::getMouseRay(const Vector2 &mouse, Vector4 &out)
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluUnProject(mouse.x, (GLdouble)viewport[3] - mouse.y - 1.0, 1.0,
                 m_modelview_matrix, m_projection_matrix, viewport,
                 &out.x, &out.y, &out.z);
    out.w = 1.0;
    out -= m_eye;
    out.normalize();
}
