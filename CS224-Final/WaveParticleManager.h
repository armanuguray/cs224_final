#ifndef WAVEPARTICLEMANAGER_H
#define WAVEPARTICLEMANAGER_H

#include <QSet>

#include "Pool.h"
#include "WaveParticle.h"

class WaveParticleManager
{
public:
    WaveParticleManager();
    virtual ~WaveParticleManager();

    void update(float time_elapsed);

    void generateUniformWave(int numParticles, const Vector2 &origin, float amplitude, float radius);

    /**
     * Renders all particles onto the screen as spheres. This is pretty much just for testing.
     */
    void drawParticles(GLUquadric *quadric);

private:
    Pool m_particleStore;
    QSet<WaveParticle *> m_liveParticles;

};

#endif // WAVEPARTICLEMANAGER_H
