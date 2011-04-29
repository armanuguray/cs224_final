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
    float dt = time_elapsed / 1000.f;   // ms -> s

    QLinkedListIterator<WaveParticle *> it(m_liveParticles);
    while (it.hasNext())
    {
        WaveParticle *p = (WaveParticle *) it.next();
        assert(p->isAlive());

        p->update(&m_liveParticles, &m_particleStore, dt);
    }
}

void WaveParticleManager::generateUniformWave(int numParticles, const Vector2 &origin, float amplitude, float radius)
{
    float dispersionAngle = 2 * M_PI / numParticles;

    for (int i = 0; i < numParticles; ++i)
    {
        float theta = 2 * M_PI * i / numParticles;

        WaveParticle *p = (WaveParticle*) m_particleStore.alloc();
        p->spawn(amplitude, radius, origin, dispersionAngle, theta);
        m_liveParticles.append(p);
    }
}

void WaveParticleManager::drawParticles(GLUquadric *quadric)
{
    // tala: is this a scale factor?
    float TEST_AMPLITUDE = 7.5f;

    QLinkedListIterator<WaveParticle*> it(m_liveParticles);
    while (it.hasNext())
    {
        WaveParticle *p = (WaveParticle*)it.next();
        assert(p->isAlive());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(p->position().x, 0.f, p->position().y);

        float lerp = .5f + .5f * (p->amplitude() / TEST_AMPLITUDE);
        glColor3f(lerp, 0.f, 1.f - lerp);

        gluSphere(quadric, 0.2, 3, 3);

        glPopMatrix();
    }
}
