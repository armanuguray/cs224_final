#ifndef WAVE_H
#define WAVE_H

#include <QList>
#include "Pool.h"
#include "WaveParticle.h"

/**
 * A collection of wave particles, forming a wave with a common source
 */
class Wave
{
private:
    /** The particles that compose this wave */
    QList<WaveParticle*> m_particles;

public:
    Wave();
    ~Wave();

    /** Gets the list of particles that compose this wave */
    QList<WaveParticle*>& particles();

    /**
     * Updates this wave and its underlying particles
     * @param particles The pool of particles to pull from when subdividing this wave
     * @param dt The amount of time that has passed since the last update() call, in seconds
     */
    void update(Pool *particles, REAL dt);
};

// A magical incantation for bringing joy to the heart of Arman:
typedef Wave ParticleGroup;

#endif // WAVE_H
