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

class QGLContext;
class QGLShaderProgram;
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

    // === mouse interaction ===
    void createWave(const Vector2 &mouse_pos);
    void turn(const Vector2 &delta);
    void pan(const Vector2 &delta);
    void zoom(REAL delta);

protected:
    // does initial OpenGL setup
    void setupGL();
    // loads shaders
    void loadShaders(const QGLContext *context);

    SkyRenderer *m_skyrenderer; // handles the rendering of the sky, including the sun.
    ProjectorCamera *m_projectorcamera; // represents the OpenGL camera. Also used for rendering the projected grid

    std::map<string, QGLShaderProgram *> m_shaderprograms; // maps a shader program object to a given name.

    GLUquadric *m_quadric;
    WaveParticleManager m_waveParticles;
    RigidBodySimulation *m_rigidbodysim;
};

#endif // DRAWENGINE_H
