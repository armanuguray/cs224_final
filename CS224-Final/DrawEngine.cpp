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

DrawEngine::DrawEngine(const QGLContext *context, int width, int height)
{
    setupGL();
    loadShaders(context);
    createFbos();
    m_skyrenderer = new SkyRenderer();
    m_projectorcamera = new ProjectorCamera(width, height);

    m_quadric = gluNewQuadric();
}

DrawEngine::~DrawEngine()
{
    delete m_skyrenderer;
    delete m_projectorcamera;
    for (std::map<string, QGLShaderProgram *>::iterator it = m_shaderprograms.begin(); it != m_shaderprograms.end(); ++it) {
        delete it->second;
    }

    for (std::map<string, QGLFramebufferObject *>::iterator it = m_fbos.begin(); it != m_fbos.end(); ++it) {
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
    glEnable(GL_TEXTURE_2D);
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

    shader = new QGLShaderProgram(context);
    if(!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/wavetest.vert")) {
        cerr << "Vertex Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/wavetest.frag")) {
        cerr << "Fragment Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    m_shaderprograms["wavetest"] = shader;

    shader = new QGLShaderProgram(context);
    if(!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/heightmap.vert")) {
        cerr << "Vertex Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/heightmap.frag")) {
        cerr << "Fragment Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    m_shaderprograms["heightmap"] = shader;
}

void DrawEngine::createFbos()
{
    m_fbos["heightmap"] = new QGLFramebufferObject(WAVE_HEIGHTMAP_RESOLUTION,
                                                   WAVE_HEIGHTMAP_RESOLUTION,
                                                   QGLFramebufferObject::NoAttachment,
                                                   GL_TEXTURE_2D,
                                                   GL_RGB16);
}

void DrawEngine::debugDrawHeightmap()
{
    int r = WAVE_HEIGHTMAP_RESOLUTION;

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, r, r, 0, -1.f, 1.f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_fbos["heightmap"]->texture());

    bool flip = false;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,flip ? 1.0f : 0.0f);
    glVertex2f(0.0f,0.0f);
    glTexCoord2f(1.0f,flip ? 1.0f : 0.0f);
    glVertex2f(  r,0.0f);
    glTexCoord2f(1.0f,flip ? 0.0f : 1.0f);
    glVertex2f(  r,   r);
    glTexCoord2f(0.0f,flip ? 0.0f : 1.0f);
    glVertex2f(0.0f,  r);
    glEnd();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void DrawEngine::drawFrame(float time_elapsed)
{
    // draw heightmap
    int r = WAVE_HEIGHTMAP_RESOLUTION,
        w = WAVE_HEIGHTMAP_WIDTH,
        h = WAVE_HEIGHTMAP_HEIGHT;

    m_fbos["heightmap"]->bind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glViewport(0, 0, r, r);
    gluOrtho2D(0, 1.f, 1.f, 0.f);

    m_shaderprograms["heightmap"]->bind();
    m_shaderprograms["heightmap"]->setUniformValue("tl", QVector3D(-w * .5f, 0, -h * .5f));
    m_shaderprograms["heightmap"]->setUniformValue("tr", QVector3D( w * .5f, 0, -h * .5f));
    m_shaderprograms["heightmap"]->setUniformValue("bl", QVector3D(-w * .5f, 0,  h * .5f));
    m_shaderprograms["heightmap"]->setUniformValue("br", QVector3D( w * .5f, 0,  h * .5f));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    bool flip = true;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    foreach (WaveParticle *particle, m_waveParticles.liveParticles())
    {
        m_shaderprograms["heightmap"]->setUniformValue("wp_pos", QVector2D(particle->position().x, particle->position().y));
        m_shaderprograms["heightmap"]->setUniformValue("wp_amplitude", (GLfloat)particle->amplitude());
        m_shaderprograms["heightmap"]->setUniformValue("wp_radius", (GLfloat)particle->radius());
        m_shaderprograms["heightmap"]->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,flip ? 1.0f : 0.0f);
        glVertex2f(0.0f, 0.0f);

        glTexCoord2f(1.0f,flip ? 1.0f : 0.0f);
        glVertex2f(1.0f, 0.0f);

        glTexCoord2f(1.0f,flip ? 0.0f : 1.0f);
        glVertex2f(1.0f, 1.0f);

        glTexCoord2f(0.0f,flip ? 0.0f : 1.0f);
        glVertex2f(0.0f, 1.0f);
        glEnd();
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shaderprograms["heightmap"]->release();

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    m_fbos["heightmap"]->release();

    glViewport(0, 0, m_projectorcamera->getWidth(), m_projectorcamera->getHeight());

    m_waveParticles.update(time_elapsed);
//    debugDrawHeightmap();
//    return;

    // render the world
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        logln(gluErrorString(err));

    // render sky
    m_skyrenderer->renderSkyBox(m_projectorcamera);

    // render water
    m_shaderprograms["wavetest"]->bind();
    m_shaderprograms["wavetest"]->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    m_shaderprograms["wavetest"]->setUniformValue("heightmap_resolution", (GLfloat)WAVE_HEIGHTMAP_RESOLUTION);
    m_shaderprograms["wavetest"]->setUniformValue("htl", QVector3D(-w * .5, 0, -h * .5));
    m_shaderprograms["wavetest"]->setUniformValue("htr", QVector3D( w * .5, 0, -h * .5));
    m_shaderprograms["wavetest"]->setUniformValue("hbl", QVector3D(-w * .5, 0,  h * .5));
    m_shaderprograms["wavetest"]->setUniformValue("hbr", QVector3D( w * .5, 0,  h * .5));
    m_shaderprograms["wavetest"]->setUniformValue("tl", QVector3D(m_projectorcamera->ul.x / m_projectorcamera->ul.w, m_projectorcamera->ul.y / m_projectorcamera->ul.w, m_projectorcamera->ul.z / m_projectorcamera->ul.w));
    m_shaderprograms["wavetest"]->setUniformValue("tr", QVector3D(m_projectorcamera->ur.x / m_projectorcamera->ur.w, m_projectorcamera->ur.y / m_projectorcamera->ur.w, m_projectorcamera->ur.z / m_projectorcamera->ur.w));
    m_shaderprograms["wavetest"]->setUniformValue("bl", QVector3D(m_projectorcamera->ll.x / m_projectorcamera->ll.w, m_projectorcamera->ll.y / m_projectorcamera->ll.w, m_projectorcamera->ll.z / m_projectorcamera->ll.w));
    m_shaderprograms["wavetest"]->setUniformValue("br", QVector3D(m_projectorcamera->lr.x / m_projectorcamera->lr.w, m_projectorcamera->lr.y / m_projectorcamera->lr.w, m_projectorcamera->lr.z / m_projectorcamera->lr.w));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbos["heightmap"]->texture());
    m_shaderprograms["wavetest"]->setUniformValue("texture", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyrenderer->getTexture());
    m_shaderprograms["wavetest"]->setUniformValue("cube", 1);

    m_projectorcamera->renderProjectedGrid();

    m_shaderprograms["wavetest"]->release();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

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
        float TEST_AMPLITUDE = 1.f;

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
        m_waveParticles.generateUniformWave(10, Vector2(intersect.x, intersect.z), -1.f, 10.f);
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
