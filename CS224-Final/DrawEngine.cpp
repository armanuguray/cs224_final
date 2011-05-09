#include "DrawEngine.h"
#include "ProjectorCamera.h"
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include "OpenGLInclude.h"
#include "SkyRenderer.h"

#include "PoolIterator.h"
#include "WaveConstants.h"
#include "RigidBody.h"
#include "RigidBodyConstants.h"

//#define SHOW_ORIGIN
//#define PARTICLE_TEST
#define DRAW_WATER

DrawEngine::DrawEngine(const QGLContext *context, int width, int height)
{
    m_waveparticles.load(context);

    m_skyrenderer = new SkyRenderer();
    m_projectorcamera = new ProjectorCamera(width, height);
    m_rigidbodysim = new RigidBodySimulation(context, m_projectorcamera, &m_waveparticles);

    setupGL();

    m_quadric = gluNewQuadric();

    m_time = 0.f;
}

DrawEngine::~DrawEngine()
{
    delete m_rigidbodysim;
    delete m_skyrenderer;
    delete m_projectorcamera;

    m_waveparticles.unload();

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

    btVector3 inertia(0, 0, 0);

    // TODO: the following is for testing only. Remove when done
    btTransform t(btQuaternion(0,0,0,1), btVector3(0,20,0));
    RigidBody *rb = m_rigidbodysim->addRigidBody(RigidBodyTypeCube, BOX_MASS, inertia, t);
}

float clamp01(float x)
{
    return x < 0.f ? 0.f : x > 1.f ? 1.f : x;
}

void DrawEngine::drawFrame(float time_elapsed)
{
    m_time += time_elapsed;

#ifdef DRAW_WATER
    m_waveparticles.moveHeightmap(m_projectorcamera);
    m_waveparticles.renderHeightmap(m_time);
    m_waveparticles.blurHeightmap();
#endif
    m_waveparticles.update(m_time, time_elapsed);

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
    m_waveparticles.renderWaves(m_projectorcamera, m_skyrenderer);
#endif

    // render rigidbodies
    m_rigidbodysim->stepSimulation(time_elapsed);
    m_rigidbodysim->renderAll();

    // generate waves
    m_rigidbodysim->generateWaves(m_waveparticles);

#ifdef SHOW_ORIGIN

    // mark the origin as a point of reference
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    glDisable(GL_CULL_FACE);
//    glBegin(GL_QUAD_STRIP);
//    {
//        glColor3f(1.0,0.0,0.0);
//        glVertex3f(-0.5, -0.5, 0.0);
//        glColor3f(0.0,1.0,0.0);
//        glVertex3f(-0.5, 0.5, 0.0);
//        glColor3f(0.0,0.0,1.0);
//        glVertex3f(0.5, -0.5, 0.0);
//        glColor3f(1.0,1.0,0.0);
//        glVertex3f(0.5, 0.5, 0.0);
//    }
//    glEnd();
//    glEnable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // z
    glPushMatrix();
    glColor3f(0, 0, 1);
    gluCylinder(m_quadric, 1, 1, 15, 20, 3);
    glRotatef(180, 1, 0, 0);
    glColor3f(0, 0, .3);
    gluCylinder(m_quadric, 1, 1, 15, 20, 3);
    glPopMatrix();

    // x
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glColor3f(1, 0, 0);
    gluCylinder(m_quadric, 1, 1, 15, 20, 3);

    glRotatef(180, 0, 1, 0);
    glColor3f(.3, 0, 0);
    gluCylinder(m_quadric, 1, 1, 15, 20, 3);
    glPopMatrix();

    // y
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glColor3f(0, 1, 0);
    gluCylinder(m_quadric, 1, 1, 15, 20, 3);

    glRotatef(180, 1, 0, 0);
    glColor3f(0, .3, 0);
    gluCylinder(m_quadric, 1, 1, 15, 20, 3);
    glPopMatrix();

    glPopMatrix();
    glEnable(GL_CULL_FACE);
#endif

#ifdef PARTICLE_TEST
    m_waveparticles.update(time_elapsed);
    m_waveparticles.drawParticlesAsSpheres(m_quadric);

    static int frame = 0;
    if (frame % 60 == 0)
    {
        int PARTICLES_PER_RING = 20;
        float TEST_AMPLITUDE = 1.f;

        m_waveparticles.generateUniformWave(PARTICLES_PER_RING, Vector2(0.f, 0.f), TEST_AMPLITUDE);
    }

    ++frame;
#endif
}

void DrawEngine::createWave(const Vector2 &mousePos)
{
    Vector4 rayDir, intersect;

    m_projectorcamera->getMouseRay(mousePos, rayDir);
    bool intersects = ProjectorCamera::intersectRayPlane(m_projectorcamera->getEye(), rayDir, 0, intersect);

    if (intersects) {
        m_waveparticles.generateUniformWave(10, Vector2(intersect.x, intersect.z), .125f, m_time);
    }
}

void DrawEngine::throwBody(const Vector2 &mousePos, const RigidBodyType &type)
{
    Vector4 rayDir;
    m_projectorcamera->getMouseRay(mousePos, rayDir);

    Vector4 eye = m_projectorcamera->getEye();

    // TODO: the following is for testing only. Remove when done
    btVector3 inertia;
    btTransform t(btQuaternion(0, 0, 0, 1), btVector3(eye.x, eye.y, eye.z));
    RigidBody *new_body = m_rigidbodysim->addRigidBody(type, BOX_MASS, inertia, t);

    new_body->getInternalRigidBody()->applyTorqueImpulse(btVector3(10, 10, 0));
    new_body->getInternalRigidBody()->applyCentralImpulse(btVector3(rayDir.x, rayDir.y, rayDir.z) * IMPULSE_SCALE);
}

void DrawEngine::turn(const Vector2 &delta)
{
    m_projectorcamera->lookVectorRotate(delta);
}

void DrawEngine::pan(const Vector2 &delta)
{
    m_projectorcamera->filmPlaneTranslate(delta);
}

void DrawEngine::zoom(REAL delta)
{
    m_projectorcamera->lookVectorTranslate(delta);
}
