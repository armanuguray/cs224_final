#ifndef WAVEPARTICLE_H
#define WAVEPARTICLE_H

#include <QSet>
#include "CS123Algebra.h"
#include "Pool.h"
#include "Poolable.h"

/**
 * A single particle in a simulated wave
 */
class WaveParticle : public Poolable
{
private:
    REAL m_amplitude;               // The amplitude created at the center of this particle in the heightmap
    Vector2 m_dispersionOrigin;     // The point at which the particle was created. Used to calculate when to subdivide
    REAL m_dispersionAngle;         // The angle between this particle and each of its neighbors. Used to calculate when to subdivide
    Vector2 m_direction;            // The direction this particle is moving
    float m_time;                   // The time this particle was spawned, in partial seconds

protected:
    void onAlloc();
    void onFree();

public:
    WaveParticle();
    ~WaveParticle();

    void spawn(const Vector2& direction, REAL amplitude, float time ,const Vector2& dispersionOrigin, REAL dispersionAngle);
    void spawn(REAL amplitude, const Vector2& dispersionOrigin, REAL dispersionAngle, REAL movementAngle, float time);

    REAL amplitude();
    void setAmplitude(REAL);

    Vector2& dispersionOrigin();
    void setDispersionOrigin(const Vector2 &);

    REAL dispersionAngle();
    void setDispersionAngle(REAL);

    Vector2& direction();
    void setDirection(const Vector2 &);

    float time();
    void setTime(float);

    /**
     * Updates this particle
     * @param dt The amount of time that has elapsed since the last update() call, in seconds
     */
    void update(QSet<WaveParticle*> *liveParticles, Pool *particles, float t, float dt);
};

#endif // WAVEPARTICLE_H
