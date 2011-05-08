#include "OpenGLInclude.h"

#include "WaveConstants.h"
#include "WaveParticle.h"
#include "WaveParticleManager.h"

WaveParticleManager::WaveParticleManager()
{
    for (int i = 0; i < WAVE_PARTICLE_COUNT; ++i) {
        m_particleStore.add(new WaveParticle());
    }
}

WaveParticleManager::~WaveParticleManager()
{
    m_particleStore.clear();
}

void WaveParticleManager::update(float time_elapsed)
{
    QSetIterator<WaveParticle *> it(m_liveParticles);
    while (it.hasNext())
    {
        WaveParticle *p = (WaveParticle *) it.next();
        assert(p->isAlive());

        p->update(&m_liveParticles, &m_particleStore, time_elapsed);
    }
}

void WaveParticleManager::generateUniformWave(int numParticles, const Vector2 &origin, float amplitude)
{
    float dispersionAngle = 2 * M_PI / numParticles;

    for (int i = 0; i < numParticles; ++i)
    {
        float theta = 2 * M_PI * i / numParticles;

        WaveParticle *p = (WaveParticle*) m_particleStore.alloc();
        if (p == NULL) return;

        p->spawn(amplitude, origin, dispersionAngle, theta);
        m_liveParticles.insert(p);
    }
}

void WaveParticleManager::drawParticlesAsSpheres(GLUquadric *quadric)
{
    QSetIterator<WaveParticle*> it(m_liveParticles);
    while (it.hasNext())
    {
        WaveParticle *p = (WaveParticle*)it.next();
        assert(p->isAlive());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(p->position().x, 0.f, p->position().y);

        float scaledAmp = p->amplitude() / 10;
//        float rlerp = .2f + (scaledAmp) * .2f;
//        float glerp = .4f + (scaledAmp) * .4f;
//        float blerp = .6f + (scaledAmp);
//        glColor3f(rlerp, glerp, blerp);
        if (scaledAmp < 0) {
            glColor3f(scaledAmp, 0, 0);
        } else {
            glColor3f(0, 0, scaledAmp);
        }

        gluSphere(quadric, WAVE_PARTICLE_RADIUS / 10.f, 3, 3);

        glPopMatrix();
    }

    //logln(m_particleStore.deadCount() << ", " << m_particleStore.liveCount() << ", " << m_particleStore.capacity());
}

void WaveParticleManager::loadShaders(const QGLContext *context)
{
    QGLShaderProgram* shader = new QGLShaderProgram(context);
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

void WaveParticleManager::createFbos()
{
    m_fbos["heightmap"] = new QGLFramebufferObject(WAVE_HEIGHTMAP_RESOLUTION,
                                                   WAVE_HEIGHTMAP_RESOLUTION,
                                                   QGLFramebufferObject::NoAttachment,
                                                   GL_TEXTURE_2D,
                                                   GL_RGB32F_ARB);
    m_fbos["convolve"] = new QGLFramebufferObject(WAVE_HEIGHTMAP_RESOLUTION,
                                                  WAVE_HEIGHTMAP_RESOLUTION,
                                                  QGLFramebufferObject::NoAttachment,
                                                  GL_TEXTURE_2D,
                                                  GL_RGB32F_ARB);
}

void WaveParticleManager::computeWeights()
{
    for (int i = 0; i <= WAVE_CONVOLUTION_KERNEL_RADIUS; ++i)
    {
        float dist = (float)i * WAVE_HEIGHTMAP_WIDTH / WAVE_HEIGHTMAP_RESOLUTION;
        float piDistOverR = M_PI * dist / WAVE_PARTICLE_RADIUS;
        float boxFactor = dist > WAVE_PARTICLE_RADIUS ? 0.f : 1.f;

        _weightsX[3 * i + 0] = -.5f * sin(piDistOverR) * (1.f + cos(piDistOverR)) * boxFactor;
        _weightsX[3 * i + 1] = .5f * (cos(piDistOverR) + 1) * boxFactor;
        _weightsX[3 * i + 2] = .25f * (1.f * cos(piDistOverR)) * (1.f * cos(piDistOverR)) * boxFactor;

        dist = (float)i * WAVE_HEIGHTMAP_HEIGHT / WAVE_HEIGHTMAP_RESOLUTION;
        piDistOverR = M_PI * dist / WAVE_PARTICLE_RADIUS;
        boxFactor = dist > WAVE_PARTICLE_RADIUS ? 0.f : 1.f;

        _weightsZ[3 * i + 0] = .25f * (1.f * cos(piDistOverR)) * (1.f * cos(piDistOverR)) * boxFactor;
        _weightsZ[3 * i + 1] = .5f * (cos(piDistOverR) + 1) * boxFactor;
        _weightsZ[3 * i + 2] = -.5f * sin(piDistOverR) * (1.f + cos(piDistOverR)) * boxFactor;
    }
}

void WaveParticleManager::load(const QGLContext *context)
{
    loadShaders(context);
    createFbos();
    computeWeights();
}

void WaveParticleManager::unload()
{
    for (std::map<string, QGLShaderProgram *>::iterator it = m_shaderprograms.begin(); it != m_shaderprograms.end(); ++it) {
        delete it->second;
    }

    for (std::map<string, QGLFramebufferObject *>::iterator it = m_fbos.begin(); it != m_fbos.end(); ++it) {
        delete it->second;
    }
}

void WaveParticleManager::debugDrawHeightmap()
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

void WaveParticleManager::renderHeightmap()
{
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

    glEnable(GL_MULTISAMPLE);

    float plotwidth = 2.f / WAVE_HEIGHTMAP_RESOLUTION;

    foreach (WaveParticle *particle, liveParticles())
    {
        glColor3f(particle->amplitude() / (float)WAVE_MAX_AMPLITUDE * .25f, -particle->amplitude() / (float)WAVE_MAX_AMPLITUDE * .25f, 0.f);
        glPushMatrix();

        float x = (particle->position().x + .5 * w) / WAVE_HEIGHTMAP_WIDTH;
        float y = 1.f - (particle->position().y + .5 * h) / WAVE_HEIGHTMAP_HEIGHT;
        glTranslatef(x, y, 0.f);

        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(plotwidth, 0.0f);
        glVertex2f(plotwidth, plotwidth);
        glVertex2f(0.0f, plotwidth);
        glEnd();

        glPopMatrix();
    }

    m_fbos["heightmap"]->release();
}

void WaveParticleManager::blurHeightmap()
{
    // Blur horizontally
    m_fbos["convolve"]->bind();
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.f, 1.f, 1.f);

    m_shaderprograms["hblur-heightmap"]->bind();
    m_shaderprograms["hblur-heightmap"]->setUniformValueArray("weights", (GLfloat*)_weightsX, WAVE_CONVOLUTION_KERNEL_WIDTH, 3);
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
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.f, 1.f);
    glTexCoord2f(0.0f,  0.0f);
    glVertex2f(0.0f, 1.f);
    glEnd();

    m_shaderprograms["hblur-heightmap"]->release();
    m_fbos["convolve"]->release();

    // Blur vertically
    m_fbos["heightmap"]->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_shaderprograms["vblur-heightmap"]->bind();
    m_shaderprograms["hblur-heightmap"]->setUniformValueArray("weights", (GLfloat*)_weightsX, WAVE_CONVOLUTION_KERNEL_WIDTH, 3);
    m_shaderprograms["vblur-heightmap"]->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    m_shaderprograms["vblur-heightmap"]->setUniformValue("heightmap_resolution", (GLfloat)WAVE_HEIGHTMAP_RESOLUTION);
    glBindTexture(GL_TEXTURE_2D, m_fbos["convolve"]->texture());
    m_shaderprograms["vblur-heightmap"]->setUniformValue("texture", 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.f, 1.f);
    glTexCoord2f(0.0f,  0.0f);
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
}

void WaveParticleManager::renderWaves(ProjectorCamera *camera, SkyRenderer *sky)
{
    int r = WAVE_HEIGHTMAP_RESOLUTION,
        w = WAVE_HEIGHTMAP_WIDTH,
        h = WAVE_HEIGHTMAP_HEIGHT;

    m_shaderprograms["wavetest"]->bind();
    m_shaderprograms["wavetest"]->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    m_shaderprograms["wavetest"]->setUniformValue("heightmap_resolution", (GLfloat)WAVE_HEIGHTMAP_RESOLUTION);
    m_shaderprograms["wavetest"]->setUniformValue("htl", QVector3D(-w * .5, 0, -h * .5));
    m_shaderprograms["wavetest"]->setUniformValue("htr", QVector3D( w * .5, 0, -h * .5));
    m_shaderprograms["wavetest"]->setUniformValue("hbl", QVector3D(-w * .5, 0,  h * .5));
    m_shaderprograms["wavetest"]->setUniformValue("hbr", QVector3D( w * .5, 0,  h * .5));
    m_shaderprograms["wavetest"]->setUniformValue("tl", QVector3D(camera->ul.x / camera->ul.w, camera->ul.y / camera->ul.w, camera->ul.z / camera->ul.w));
    m_shaderprograms["wavetest"]->setUniformValue("tr", QVector3D(camera->ur.x / camera->ur.w, camera->ur.y / camera->ur.w, camera->ur.z / camera->ur.w));
    m_shaderprograms["wavetest"]->setUniformValue("bl", QVector3D(camera->ll.x / camera->ll.w, camera->ll.y / camera->ll.w, camera->ll.z / camera->ll.w));
    m_shaderprograms["wavetest"]->setUniformValue("br", QVector3D(camera->lr.x / camera->lr.w, camera->lr.y / camera->lr.w, camera->lr.z / camera->lr.w));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbos["heightmap"]->texture());
    m_shaderprograms["wavetest"]->setUniformValue("texture", 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sky->getTexture());
    m_shaderprograms["wavetest"]->setUniformValue("cube", 1);

    camera->renderProjectedGrid();

    m_shaderprograms["wavetest"]->release();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void WaveParticleManager::draw(ProjectorCamera* camera, SkyRenderer *sky)
{
    renderHeightmap();
    blurHeightmap();
    renderWaves(camera, sky);
}
