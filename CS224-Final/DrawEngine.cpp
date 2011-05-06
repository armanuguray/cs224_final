#include "DrawEngine.h"
#include "ProjectorCamera.h"
#include <QGLShaderProgram>
#include "OpenGLInclude.h"
#include "SkyRenderer.h"

#include "PoolIterator.h"
#include "WaveConstants.h"
#include "RigidBody.h"

#define SHOW_ORIGIN
#define PARTICLE_TEST

DrawEngine::DrawEngine(const QGLContext *context, int width, int height)
{
    m_skyrenderer = new SkyRenderer();
    m_projectorcamera = new ProjectorCamera(width, height);
    m_rigidbodysim = new RigidBodySimulation(context, m_projectorcamera);

    setupGL();
    loadShaders(context);

    m_quadric = gluNewQuadric();
}

DrawEngine::~DrawEngine()
{
    delete m_rigidbodysim;
    delete m_skyrenderer;
    delete m_projectorcamera;
    for (std::map<string, QGLShaderProgram *>::iterator it = m_shaderprograms.begin(); it != m_shaderprograms.end(); ++it) {
        delete it->second;
    }

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

    btVector3 inertia(0, 0, 0);

    // TODO: the following is for testing only. Remove when done
    btTransform t(btQuaternion(0,0,0,1), btVector3(0,20,0));
    RigidBody *rb = m_rigidbodysim->addRigidBody(RigidBodyTypeCube, 20, inertia, t);
    // add torque for fun
    rb->getInternalRigidBody()->applyTorqueImpulse(btVector3(10,10,0));
}

void DrawEngine::loadShaders(const QGLContext *context)
{
    QGLShaderProgram *shader = new QGLShaderProgram(context);
    if(!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/fresnel.vert")) {
        cerr << "Vertex Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/fresnel.frag")) {
        cerr << "Fragment Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    m_shaderprograms["fresnel"] = shader;
}

void DrawEngine::drawFrame(float time_elapsed)
{    
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    if (glGetError() != GL_NO_ERROR) logln("Error");

    // render sky
    m_skyrenderer->renderSkyBox(m_projectorcamera);

    // render water
    m_shaderprograms["fresnel"]->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyrenderer->getTexture());
    m_shaderprograms["fresnel"]->setUniformValue("cube", 0);
    m_projectorcamera->renderProjectedGrid();
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    m_shaderprograms["fresnel"]->release();

    // render rigidbodies
    m_rigidbodysim->stepSimulation();
    m_rigidbodysim->renderAll();

    // mark the origin as a point of reference
#ifdef SHOW_ORIGIN
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
    m_waveParticles.drawParticles(m_quadric);

    static int frame = 0;
    if (frame % 60 == 0)
    {
        int PARTICLES_PER_RING = 20;
        float TEST_AMPLITUDE = 15.f;

        m_waveParticles.generateUniformWave(PARTICLES_PER_RING, Vector2(0.f, 0.f), TEST_AMPLITUDE, 10.f);
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
        m_waveParticles.generateUniformWave(10, Vector2(intersect.x, intersect.z), 10, 10.f);
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
