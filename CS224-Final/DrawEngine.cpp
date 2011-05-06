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
    computeWeights();

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

    shader = new QGLShaderProgram(context);
    if(!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/vblur-heightmap.vert")) {
        cerr << "Vertex Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/vblur-heightmap.frag")) {
        cerr << "Fragment Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    m_shaderprograms["vblur-heightmap"] = shader;

    shader = new QGLShaderProgram(context);
    if(!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/hblur-heightmap.vert")) {
        cerr << "Vertex Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/hblur-heightmap.frag")) {
        cerr << "Fragment Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    m_shaderprograms["hblur-heightmap"] = shader;
}

void DrawEngine::createFbos()
{
    m_fbos["heightmap"] = new QGLFramebufferObject(WAVE_HEIGHTMAP_RESOLUTION,
                                                   WAVE_HEIGHTMAP_RESOLUTION,
                                                   QGLFramebufferObject::NoAttachment,
                                                   GL_TEXTURE_2D,
                                                   GL_RGBA);
    m_fbos["convolve"] = new QGLFramebufferObject(WAVE_HEIGHTMAP_RESOLUTION,
                                                  WAVE_HEIGHTMAP_RESOLUTION,
                                                  QGLFramebufferObject::NoAttachment,
                                                  GL_TEXTURE_2D,
                                                  GL_RGBA);
}

void DrawEngine::computeWeights()
{
    int middle = WAVE_CONVOLUTION_KERNEL_RADIUS;
    for (int i = 0; i <= WAVE_CONVOLUTION_KERNEL_RADIUS; ++i)
    {
        // X
        float dist = (float)i * WAVE_HEIGHTMAP_WIDTH / WAVE_HEIGHTMAP_RESOLUTION;
        float value = .5f * cos(M_PI * dist / WAVE_PARTICLE_RADIUS) * (dist > WAVE_PARTICLE_RADIUS ? 0.f : 1.f);
        _verticalWeightsX[middle + i] = value;
        _verticalWeightsX[middle - i] = value;

        // Z
        dist = (float)i * WAVE_HEIGHTMAP_HEIGHT / WAVE_HEIGHTMAP_RESOLUTION;
        value = .5f * cos(M_PI * dist / WAVE_PARTICLE_RADIUS) * (dist > WAVE_PARTICLE_RADIUS ? 0.f : 1.f);
        _verticalWeightsZ[middle + i] = value;
        _verticalWeightsZ[middle - i] = value;
    }
}

void DrawEngine::debugDrawHeightmap()
{
    int r = WAVE_HEIGHTMAP_RESOLUTION;

    glViewport(0, 0, r, r);

    glColor3f(1.f, 1.f, 1.f);
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
//    glBindTexture(GL_TEXTURE_2D, m_fbos["convolve"]->texture());

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

float clamp01(float x)
{
    return x < 0.f ? 0.f : x > 1.f ? 1.f : x;
}

void DrawEngine::drawFrame(float time_elapsed)
{
    // draw
    int r = WAVE_HEIGHTMAP_RESOLUTION,
        w = WAVE_HEIGHTMAP_WIDTH,
        h = WAVE_HEIGHTMAP_HEIGHT;

    // Plot the particles (convolve later)
    m_fbos["heightmap"]->bind();
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.f, 1.f, 1.f, 0.f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, r, r);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_TEXTURE_2D);

    bool flip = true;
    float plotwidth = 1.f / WAVE_HEIGHTMAP_RESOLUTION;

    foreach (WaveParticle *particle, m_waveParticles.liveParticles())
    {
        glColor3f(clamp01(particle->amplitude() / (float)WAVE_MAX_AMPLITUDE), clamp01(-particle->amplitude() / (float)WAVE_MAX_AMPLITUDE), 0.f);
        glPushMatrix();

        float x = (particle->position().x + .5 * w) / WAVE_HEIGHTMAP_WIDTH;
        float y = 1.f - (particle->position().y + .5 * h) / WAVE_HEIGHTMAP_HEIGHT;
        glTranslatef(x, y, 0.f);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,flip ? 1.0f : 0.0f);
        glVertex2f(0.0f, 0.0f);

        glTexCoord2f(1.0f,flip ? 1.0f : 0.0f);
        glVertex2f(plotwidth, 0.0f);

        glTexCoord2f(1.0f,flip ? 0.0f : 1.0f);
        glVertex2f(plotwidth, plotwidth);

        glTexCoord2f(0.0f,flip ? 0.0f : 1.0f);
        glVertex2f(0.0f, plotwidth);
        glEnd();

        glPopMatrix();
    }

    m_fbos["heightmap"]->release();

    // Blur horizontally
    m_fbos["convolve"]->bind();
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.f, 1.f, 1.f);

    m_shaderprograms["hblur-heightmap"]->bind();
    m_shaderprograms["hblur-heightmap"]->setUniformValueArray("weights", (GLfloat*)_verticalWeightsX, WAVE_CONVOLUTION_KERNEL_WIDTH, 1);
    m_shaderprograms["hblur-heightmap"]->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    m_shaderprograms["hblur-heightmap"]->setUniformValue("heightmap_resolution", (GLfloat)WAVE_HEIGHTMAP_RESOLUTION);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbos["heightmap"]->texture());
    m_shaderprograms["hblur-heightmap"]->setUniformValue("texture", 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, flip ? 1.0f : 0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(1.0f, flip ? 1.0f : 0.0f);
    glVertex2f(1.f, 0.0f);

    glTexCoord2f(1.0f, flip ? 0.0f : 1.0f);
    glVertex2f(1.f, 1.f);

    glTexCoord2f(0.0f, flip ? 0.0f : 1.0f);
    glVertex2f(0.0f, 1.f);
    glEnd();

    m_shaderprograms["hblur-heightmap"]->release();
    m_fbos["convolve"]->release();

    // Blur vertically
    m_fbos["heightmap"]->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_shaderprograms["vblur-heightmap"]->bind();
    m_shaderprograms["vblur-heightmap"]->setUniformValueArray("weights", (GLfloat*)_verticalWeightsZ, WAVE_CONVOLUTION_KERNEL_WIDTH, 1);
    m_shaderprograms["vblur-heightmap"]->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    m_shaderprograms["vblur-heightmap"]->setUniformValue("heightmap_resolution", (GLfloat)WAVE_HEIGHTMAP_RESOLUTION);
    glBindTexture(GL_TEXTURE_2D, m_fbos["convolve"]->texture());
    m_shaderprograms["vblur-heightmap"]->setUniformValue("texture", 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, flip ? 1.0f : 0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(1.0f, flip ? 1.0f : 0.0f);
    glVertex2f(1.f, 0.0f);

    glTexCoord2f(1.0f, flip ? 0.0f : 1.0f);
    glVertex2f(1.f, 1.f);

    glTexCoord2f(0.0f, flip ? 0.0f : 1.0f);
    glVertex2f(0.0f, 1.f);
    glEnd();

    m_shaderprograms["vblur-heightmap"]->release();
    m_fbos["heightmap"]->release();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    m_waveParticles.update(time_elapsed);
//    debugDrawHeightmap();
//    return;

    glViewport(0, 0, m_projectorcamera->getWidth(), m_projectorcamera->getHeight());

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyrenderer->getTexture());
    m_shaderprograms["wavetest"]->setUniformValue("cube", 1);

    m_projectorcamera->renderProjectedGrid();

    m_shaderprograms["wavetest"]->release();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

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
        m_waveParticles.generateUniformWave(10, Vector2(intersect.x, intersect.z), -.25f, 7.f);
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
