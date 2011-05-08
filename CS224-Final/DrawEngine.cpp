#include "DrawEngine.h"
#include "ProjectorCamera.h"
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include "OpenGLInclude.h"
#include "SkyRenderer.h"

#include "PoolIterator.h"
#include "WaveConstants.h"

#define SHOW_ORIGIN
//#define PARTICLE_TEST
#define DRAW_WATER

DrawEngine::DrawEngine(const QGLContext *context, int width, int height)
{
    setupGL();
    m_waveParticles.load(context);

    m_skyrenderer = new SkyRenderer();
    m_projectorcamera = new ProjectorCamera(width, height);

    m_quadric = gluNewQuadric();
}

DrawEngine::~DrawEngine()
{
    delete m_skyrenderer;
    delete m_projectorcamera;

    m_waveParticles.unload();

    gluDeleteQuadric(m_quadric);
    m_quadric = 0;
}

void DrawEngine::resize(REAL width, REAL height)
{
    m_projectorcamera->resize(width, height);
}

void DrawEngine::setupGL()
{
    // TODO Set this up before and after each draw.
    glClearColor(0.0, 0.0, 0.0, 0);
    glFrontFace(GL_CCW);
    glDisable(GL_DITHER);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_2D);
}

float clamp01(float x)
{
    return x < 0.f ? 0.f : x > 1.f ? 1.f : x;
}

void DrawEngine::drawFrame(float time_elapsed)
{
#ifdef DRAW_WATER
    m_waveParticles.renderHeightmap();
    m_waveParticles.blurHeightmap();
#endif
    m_waveParticles.update(time_elapsed);

    // render the world
    glViewport(0, 0, m_projectorcamera->getWidth(), m_projectorcamera->getHeight());
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        logln(gluErrorString(err));

    // render sky
    m_skyrenderer->renderSkyBox(m_projectorcamera);

    // render water
#ifdef DRAW_WATER
    m_waveParticles.renderWaves(m_projectorcamera, m_skyrenderer);
#endif

#ifdef SHOW_ORIGIN
    // mark the origin as a point of reference
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glBegin(GL_QUAD_STRIP);
    {
        glColor3f(1.0,0.0,0.0);
        glVertex3f(-0.5, -0.5, 0.0);
        glColor3f(0.0,1.0,0.0);
        glVertex3f(-0.5, 0.5, 0.0);
        glColor3f(0.0,0.0,1.0);
        glVertex3f(0.5, -0.5, 0.0);
        glColor3f(1.0,1.0,0.0);
        glVertex3f(0.5, 0.5, 0.0);
    }
    glEnd();
    glEnable(GL_CULL_FACE);
#endif

#ifdef PARTICLE_TEST
    m_waveParticles.update(time_elapsed);
    m_waveParticles.drawParticlesAsSpheres(m_quadric);

    static int frame = 0;
    if (frame % 60 == 0)
    {
        int PARTICLES_PER_RING = 20;
        float TEST_AMPLITUDE = 1.f;

        m_waveParticles.generateUniformWave(PARTICLES_PER_RING, Vector2(0.f, 0.f), TEST_AMPLITUDE);
    }

    ++frame;
#endif
}

void DrawEngine::mouse_down(Vector2 &mouse_pos, MouseButton button)
{
    switch (button)
    {
    case MouseButtonCTRLLeft:
        interact(mouse_pos);
        break;
    default:
        break;
    }
}

void DrawEngine::interact(Vector2 &mouse_pos)
{
    Vector4 rayDir, intersect;

    m_projectorcamera->getMouseRay(mouse_pos, rayDir);
    bool intersects = ProjectorCamera::intersectRayPlane(m_projectorcamera->getEye(), rayDir, 0, intersect);

    if (intersects) {
        m_waveParticles.generateUniformWave(10, Vector2(intersect.x, intersect.z), .125f);
    }
}

void DrawEngine::mouse_scroll(REAL delta)
{
    m_projectorcamera->lookVectorTranslate(delta);
}

void DrawEngine::mouse_dragged(Vector2 &new_mouse_pos, Vector2 &delta, MouseButton button)
{
    switch (button)
    {
    case MouseButtonLeft:
        m_projectorcamera->lookVectorRotate(delta);
        break;
    case MouseButtonRight:
        m_projectorcamera->filmPlaneTranslate(delta*1.5);
        break;
    case MouseButtonCTRLLeft:
        this->mouse_down(new_mouse_pos, button);
    default:
        break;
    }
}
