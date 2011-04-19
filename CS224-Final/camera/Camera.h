/**
 * @author auguray
 * @paragraph Interactive Camera
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "OpenGLInclude.h"
#include "CS123Algebra.h"

class Camera
{
public:
    Camera (int width, int height);

    // getters for primary Camera components
    inline Vector4 getEye() const { return m_eye; }
    inline Vector4 getLook() const { return m_look; }
    inline Vector4 getUp() const { return m_up; }

    inline Vector4 getU() const { return m_look.cross(m_up).getNormalized(); }
    inline Vector4 getV() const { return getU().cross(m_look).getNormalized(); }
    inline Vector4 getW() const { return -m_look.getNormalized(); }

    // getters to primary camera matrices
    inline Matrix4x4 getModelViewMatrix() const { return Matrix4x4((REAL*)m_modelview_matrix).getTranspose(); }
    inline Matrix4x4 getProjectionMatrix() const { return Matrix4x4((REAL*)m_projection_matrix).getTranspose(); }

    // should be called when screen size changes to retain aspect ratio
    void resize(int width, int height);

    // primary interaction
    void filmPlaneTranslate(const Vector2 &delta);
    void lookVectorRotate(const Vector2 &delta);
    void lookVectorTranslate(REAL delta);

protected:

    virtual void loadMatrices();

    Vector4 m_eye, m_look, m_up;
    GLfloat m_fovy, m_near, m_far;
    int m_width, m_height;

    GLdouble m_modelview_matrix[16],
            m_projection_matrix[16];
};

#endif // CAMERA_H
