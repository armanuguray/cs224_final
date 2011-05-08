#ifndef DRAWENGINE_H
#define DRAWENGINE_H

#include "OpenGLInclude.h"
#include "CS123Algebra.h"
#include <map>
#include <string>

#include "Pool.h"
#include "WaveParticle.h"
#include "WaveParticleManager.h"
#include "RigidBodySimulation.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "WaveConstants.h"

class QGLContext;
class QGLShaderProgram;
class QGLFramebufferObject;
class ProjectorCamera;
class SkyRenderer;

typedef enum {
    MouseButtonLeft,
    MouseButtonRight,
    MouseButtonCTRLLeft,
} MouseButton;

class DrawEngine
{
public:
    DrawEngine(const QGLContext *context, int width, int height);
    ~DrawEngine();

    // draws to the rendering context. Should be called each frame
    void drawFrame(float time_elapsed);
    // should be called when the window gets resized
    void resize(REAL width, REAL height);

    // === mouse i`nteraction ===
    // controls that interact with the water/objects
    void mouse_down(Vector2 &mouse_pos, MouseButton button);

    // controls that move the camera
    void mouse_scroll(REAL delta);
    void mouse_dragged(Vector2 &new_mouse_pos, Vector2 &delta, MouseButton button);

    // === mouse interaction ===
    void createWave(const Vector2 &mouse_pos);
    void throwBody(const Vector2 &mouse_pos, const RigidBodyType &type);
    void turn(const Vector2 &delta);
    void pan(const Vector2 &delta);
    void zoom(REAL delta);

protected:
    // does initial OpenGL setup
    void setupGL();

    SkyRenderer *m_skyrenderer; // handles the rendering of the sky, including the sun.
    ProjectorCamera *m_projectorcamera; // represents the OpenGL camera. Also used for rendering the projected grid

    GLUquadric *m_quadric;
    WaveParticleManager m_waveparticles;
    RigidBodySimulation *m_rigidbodysim;
};

#endif // DRAWENGINE_H
