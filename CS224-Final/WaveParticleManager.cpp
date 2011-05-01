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

    QSetIterator<WaveParticle *> it(m_liveParticles);
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
        if (p == NULL) return;

        p->spawn(amplitude, radius, origin, dispersionAngle, theta);
        m_liveParticles.insert(p);
    }
}

void WaveParticleManager::drawParticles(GLUquadric *quadric)
{
    QSetIterator<WaveParticle*> it(m_liveParticles);
    while (it.hasNext())
    {
        WaveParticle *p = (WaveParticle*)it.next();
        assert(p->isAlive());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(p->position().x, 0.f, p->position().y);

        float scaledAmp = p->amplitude() / 20;
        float rlerp = .2f + (scaledAmp) * .2f;
        float glerp = .4f + (scaledAmp) * .4f;
        float blerp = .6f + (scaledAmp);
        glColor3f(rlerp, glerp, blerp);

        gluSphere(quadric, p->radius() / 10.f, 3, 3);

        glPopMatrix();
    }

    //logln(m_particleStore.deadCount() << ", " << m_particleStore.liveCount() << ", " << m_particleStore.capacity());
}
