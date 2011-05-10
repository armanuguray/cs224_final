#include "OpenGLInclude.h"

#include "WaveConstants.h"
#include "WaveParticle.h"
#include "WaveParticleManager.h"

#include "Settings.h"

WaveParticleManager::WaveParticleManager()
{
    for (int i = 0; i < WAVE_PARTICLE_COUNT; ++i) {
        m_particleStore.add(new WaveParticle());
    }

    m_heightmapX = 0.f;
    m_heightmapZ = 0.f;
}

WaveParticleManager::~WaveParticleManager()
{
    m_particleStore.clear();
}

void WaveParticleManager::update(float time, float time_elapsed)
{
    QSetIterator<WaveParticle *> it(m_liveParticles);
    while (it.hasNext())
    {
        WaveParticle *p = (WaveParticle *) it.next();
        assert(p->isAlive());

        p->update(&m_liveParticles, &m_particleStore, time, time_elapsed);
    }
}

void WaveParticleManager::generateUniformWave(int numParticles, const Vector2 &origin, float amplitude, float time)
{
    float dispersionAngle = 2 * M_PI / numParticles;

    for (int i = 0; i < numParticles; ++i)
    {
        float theta = 2 * M_PI * i / numParticles;

        WaveParticle *p = (WaveParticle*) m_particleStore.alloc();
        if (p == NULL) return;

        p->spawn(amplitude, origin, dispersionAngle, theta, time);
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
//        glTranslatef(p->position().x, 0.f, p->position().y);

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

    shader = new QGLShaderProgram(context);
    if(!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/plot-heightmap.vert")) {
        cerr << "Vertex Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/plot-heightmap.frag")) {
        cerr << "Fragment Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    m_shaderprograms["plot-heightmap"] = shader;

    shader = new QGLShaderProgram(context);
    if(!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/plot-ambient.vert")) {
        cerr << "Vertex Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/plot-ambient.frag")) {
        cerr << "Fragment Shader:\n" << shader->log().data() << endl;
        exit(1);
    }
    m_shaderprograms["plot-ambient"] = shader;
}

void WaveParticleManager::createFbos()
{
    // Heightmap / Velocity
    glGenFramebuffers(1, &m_heightVelocityFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_heightVelocityFBO);
    glGenTextures(2, m_heightVelocityTargets);
    glBindTexture(GL_TEXTURE_2D, m_heightVelocityTargets[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, WAVE_HEIGHTMAP_RESOLUTION, WAVE_HEIGHTMAP_RESOLUTION, 0, GL_RGBA, GL_FLOAT, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_heightVelocityTargets[0], 0);
    glBindTexture(GL_TEXTURE_2D, m_heightVelocityTargets[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WAVE_HEIGHTMAP_RESOLUTION, WAVE_HEIGHTMAP_RESOLUTION, 0, GL_RGBA, GL_FLOAT, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_heightVelocityTargets[1], 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convolution
    glGenFramebuffers(1, &m_convolutionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_convolutionFBO);
    glGenTextures(2, m_convolutionTargets);
    glBindTexture(GL_TEXTURE_2D, m_convolutionTargets[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, WAVE_HEIGHTMAP_RESOLUTION, WAVE_HEIGHTMAP_RESOLUTION, 0, GL_RGBA, GL_FLOAT, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_convolutionTargets[0], 0);
    glBindTexture(GL_TEXTURE_2D, m_convolutionTargets[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WAVE_HEIGHTMAP_RESOLUTION, WAVE_HEIGHTMAP_RESOLUTION, 0, GL_RGBA, GL_FLOAT, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_convolutionTargets[1], 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaveParticleManager::computeWeights()
{
    for (int i = 0; i <= WAVE_CONVOLUTION_KERNEL_RADIUS; ++i)
    {
        float dist = (float)i * WAVE_HEIGHTMAP_WIDTH / WAVE_HEIGHTMAP_RESOLUTION;
        float piVOverR = M_PI * WAVE_SPEED / WAVE_PARTICLE_RADIUS;
        float piDistOverR = M_PI * dist / WAVE_PARTICLE_RADIUS;
        float boxFactor = dist > WAVE_PARTICLE_RADIUS ? 0.f : 1.f;

        m_heightWeightsX[3 * i + 0] = -.5f * sin(piDistOverR) * (1.f + cos(piDistOverR)) * boxFactor;
        m_heightWeightsX[3 * i + 1] = .5f * (cos(piDistOverR) + 1) * boxFactor;
        m_heightWeightsX[3 * i + 2] = .25f * (1.f * cos(piDistOverR)) * (1.f * cos(piDistOverR)) * boxFactor;

        m_velocityWeightsX[2 * i + 0] = -.5f * (cos(2 * piDistOverR) + cos(piDistOverR));// * piVOverR;
        m_velocityWeightsX[2 * i + 1] = .25f * (cos(piDistOverR) + 1) * (cos(piDistOverR) + 1);

        dist = (float)i * WAVE_HEIGHTMAP_HEIGHT / WAVE_HEIGHTMAP_RESOLUTION;
        piDistOverR = M_PI * dist / WAVE_PARTICLE_RADIUS;
        boxFactor = dist > WAVE_PARTICLE_RADIUS ? 0.f : 1.f;

        m_heightWeightsZ[3 * i + 0] = .25f * (1.f * cos(piDistOverR)) * (1.f * cos(piDistOverR)) * boxFactor;
        m_heightWeightsZ[3 * i + 1] = .5f * (cos(piDistOverR) + 1) * boxFactor;
        m_heightWeightsZ[3 * i + 2] = -.5f * sin(piDistOverR) * (1.f + cos(piDistOverR)) * boxFactor;

        m_velocityWeightsZ[2 * i + 0] = .25f * (cos(piDistOverR) + 1) * (cos(piDistOverR) + 1);
        m_velocityWeightsZ[2 * i + 1] = -.5f * (cos(2 * piDistOverR) + cos(piDistOverR));// * piVOverR;
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

    glDeleteFramebuffers(1, &m_heightVelocityFBO);
    glDeleteFramebuffers(1, &m_convolutionFBO);
    glDeleteTextures(2, m_heightVelocityTargets);
    glDeleteTextures(2, m_convolutionTargets);
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
    glBindTexture(GL_TEXTURE_2D, m_heightVelocityTargets[0]);
//    glBindTexture(GL_TEXTURE_2D, m_fbos["heightmap"]->texture());
//    glBindTexture(GL_TEXTURE_2D, m_fbos["heightConvolve"]->texture());

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

void WaveParticleManager::renderHeightmap(float t)
{
    int r = WAVE_HEIGHTMAP_RESOLUTION,
        w = WAVE_HEIGHTMAP_WIDTH,
        h = WAVE_HEIGHTMAP_HEIGHT;

    glBindFramebuffer(GL_FRAMEBUFFER, m_heightVelocityFBO);
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);
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

    if (settings.ambient_waves)
    {
        // Ambient waves
        m_shaderprograms["plot-ambient"]->bind();

        m_shaderprograms["plot-ambient"]->setUniformValue("heightmap_center", QVector2D(m_heightmapX, m_heightmapZ));
        m_shaderprograms["plot-ambient"]->setUniformValue("heightmap_size", QVector2D(w, h));
        m_shaderprograms["plot-ambient"]->setUniformValue("heightmap_resolution", (GLfloat)WAVE_HEIGHTMAP_RESOLUTION);
        m_shaderprograms["plot-ambient"]->setUniformValue("max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
        m_shaderprograms["plot-ambient"]->setUniformValue("flow_direction", QVector2D(1.f, 0.f));
        m_shaderprograms["plot-ambient"]->setUniformValue("velocity", (GLfloat)WAVE_SPEED * .1f);
        m_shaderprograms["plot-ambient"]->setUniformValue("time", (GLfloat)t);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(0.f, 0.f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(  r, 0.f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(  r,   r);
        glTexCoord2f(0.0f,  0.0f);
        glVertex2f(0.f,   r);
        glEnd();

        m_shaderprograms["plot-ambient"]->release();
    }

    // Wave particle-waves
    float plotwidth = 2.f / WAVE_HEIGHTMAP_RESOLUTION;

    m_shaderprograms["plot-heightmap"]->bind();
    m_shaderprograms["plot-heightmap"]->setUniformValue("max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    m_shaderprograms["plot-heightmap"]->setUniformValue("heightmap_center", QVector2D(m_heightmapX, m_heightmapZ));
    m_shaderprograms["plot-heightmap"]->setUniformValue("heightmap_size", QVector2D(w, h));
    m_shaderprograms["plot-heightmap"]->setUniformValue("time", (GLfloat)t);
    m_shaderprograms["plot-heightmap"]->setUniformValue("velocity", (GLfloat)WAVE_SPEED);

    QVector2D pos, dir;

    foreach (WaveParticle *particle, liveParticles())
    {
        if (particle->amplitude() < WAVE_MIN_AMPLITUDE)
        {
            m_liveParticles.remove(particle);
            m_particleStore.free(particle);
        }

        pos.setX(particle->dispersionOrigin().x);
        pos.setY(particle->dispersionOrigin().y);
        dir.setX(particle->direction().x);
        dir.setY(particle->direction().y);

        m_shaderprograms["plot-heightmap"]->setUniformValue("wp_amplitude", (GLfloat)particle->amplitude());
        m_shaderprograms["plot-heightmap"]->setUniformValue("wp_spawn_point", pos);
        m_shaderprograms["plot-heightmap"]->setUniformValue("wp_time", (GLfloat)particle->time());
        m_shaderprograms["plot-heightmap"]->setUniformValue("wp_direction", dir);

        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(plotwidth, 0.0f);
        glVertex2f(plotwidth, plotwidth);
        glVertex2f(0.0f, plotwidth);
        glEnd();
    }

    m_shaderprograms["plot-heightmap"]->release();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

void WaveParticleManager::blurHeightmap()
{
    // Blur horizontally    
    glBindFramebuffer(GL_FRAMEBUFFER, m_convolutionFBO);
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.f, 1.f, 1.f);

    m_shaderprograms["hblur-heightmap"]->bind();
    m_shaderprograms["hblur-heightmap"]->setUniformValueArray("heightWeights", (GLfloat*)m_heightWeightsX, WAVE_CONVOLUTION_KERNEL_WIDTH, 3);
    m_shaderprograms["hblur-heightmap"]->setUniformValueArray("velocityWeights", (GLfloat*)m_velocityWeightsX, WAVE_CONVOLUTION_KERNEL_WIDTH, 2);
    m_shaderprograms["hblur-heightmap"]->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    m_shaderprograms["hblur-heightmap"]->setUniformValue("heightmap_resolution", (GLfloat)WAVE_HEIGHTMAP_RESOLUTION);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_heightVelocityTargets[0]);
    m_shaderprograms["hblur-heightmap"]->setUniformValue("texture", 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_heightVelocityTargets[1]);
    m_shaderprograms["hblur-heightmap"]->setUniformValue("velocity", 1);
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

    // Blur vertically
    glBindFramebuffer(GL_FRAMEBUFFER, m_heightVelocityFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    m_shaderprograms["vblur-heightmap"]->bind();
    m_shaderprograms["vblur-heightmap"]->setUniformValueArray("heightWeights", (GLfloat*)m_heightWeightsZ, WAVE_CONVOLUTION_KERNEL_WIDTH, 3);
    m_shaderprograms["vblur-heightmap"]->setUniformValueArray("velocityWeights", (GLfloat*)m_velocityWeightsZ, WAVE_CONVOLUTION_KERNEL_WIDTH, 2);
    m_shaderprograms["vblur-heightmap"]->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    m_shaderprograms["vblur-heightmap"]->setUniformValue("heightmap_resolution", (GLfloat)WAVE_HEIGHTMAP_RESOLUTION);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_convolutionTargets[0]);
    m_shaderprograms["vblur-heightmap"]->setUniformValue("texture", 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_convolutionTargets[1]);
    m_shaderprograms["vblur-heightmap"]->setUniformValue("velocity", 1);
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
    glActiveTexture(GL_TEXTURE0);

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
    int w = WAVE_HEIGHTMAP_WIDTH,
        h = WAVE_HEIGHTMAP_HEIGHT;

    m_shaderprograms["wavetest"]->bind();
    m_shaderprograms["wavetest"]->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    m_shaderprograms["wavetest"]->setUniformValue("heightmap_resolution", (GLfloat)WAVE_HEIGHTMAP_RESOLUTION);
    m_shaderprograms["wavetest"]->setUniformValue("htl", QVector3D(-w * .5 + m_heightmapX, 0, -h * .5 + m_heightmapZ));
    m_shaderprograms["wavetest"]->setUniformValue("htr", QVector3D( w * .5 + m_heightmapX, 0, -h * .5 + m_heightmapZ));
    m_shaderprograms["wavetest"]->setUniformValue("hbl", QVector3D(-w * .5 + m_heightmapX, 0,  h * .5 + m_heightmapZ));
    m_shaderprograms["wavetest"]->setUniformValue("hbr", QVector3D( w * .5 + m_heightmapX, 0,  h * .5 + m_heightmapZ));
    m_shaderprograms["wavetest"]->setUniformValue("tl", QVector3D(camera->ul.x / camera->ul.w, camera->ul.y / camera->ul.w, camera->ul.z / camera->ul.w));
    m_shaderprograms["wavetest"]->setUniformValue("tr", QVector3D(camera->ur.x / camera->ur.w, camera->ur.y / camera->ur.w, camera->ur.z / camera->ur.w));
    m_shaderprograms["wavetest"]->setUniformValue("bl", QVector3D(camera->ll.x / camera->ll.w, camera->ll.y / camera->ll.w, camera->ll.z / camera->ll.w));
    m_shaderprograms["wavetest"]->setUniformValue("br", QVector3D(camera->lr.x / camera->lr.w, camera->lr.y / camera->lr.w, camera->lr.z / camera->lr.w));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_heightVelocityTargets[0]);
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

void WaveParticleManager::draw(ProjectorCamera* camera, SkyRenderer *sky, float t)
{
    renderHeightmap(t);
    blurHeightmap();
    renderWaves(camera, sky);
}

void WaveParticleManager::moveHeightmap(ProjectorCamera *camera)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    Vector2 screenleft (        0.0, camera->getEye().z > 0.0 ? viewport[3] : 0.0);
    Vector2 screenright(viewport[2], camera->getEye().z > 0.0 ? viewport[3] : 0.0);

    Vector4 leftray, rightray;
    camera->getMouseRay(screenleft, leftray);
    camera->getMouseRay(screenright, rightray);

    Vector4 left, right;
    (ProjectorCamera::intersectRayPlane(camera->getEye(), leftray, 0.0, left));
    (ProjectorCamera::intersectRayPlane(camera->getEye(), rightray, 0.0, right));

    Vector4 test = right - left;
    if (test.x > 0.0 && test.z > 0.0)
    {
        m_heightmapX =  left.x + .4 * WAVE_HEIGHTMAP_WIDTH;
        m_heightmapZ = right.z - .4 * WAVE_HEIGHTMAP_HEIGHT;
    }
    else if (test.x < 0.0 && test.z > 0.0)
    {
        m_heightmapX = right.x + .4 * WAVE_HEIGHTMAP_WIDTH;
        m_heightmapZ =  left.z + .4 * WAVE_HEIGHTMAP_HEIGHT;
    }
    else if (test.x < 0.0 && test.z < 0.0)
    {
        m_heightmapX =  left.x - .4 * WAVE_HEIGHTMAP_WIDTH;
        m_heightmapZ = right.z + .4 * WAVE_HEIGHTMAP_HEIGHT;
    }
    else if (test.x > 0.0 && test.z < 0.0)
    {
        m_heightmapX = right.x - .4 * WAVE_HEIGHTMAP_WIDTH;
        m_heightmapZ =  left.z - .4 * WAVE_HEIGHTMAP_HEIGHT;
    }
}
