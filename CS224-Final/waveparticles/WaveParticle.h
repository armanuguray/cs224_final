#ifndef WAVEPARTICLE_H
#define WAVEPARTICLE_H

#include "CS123Algebra.h"
#include "Poolable.h"

/**
 * A single particle in a simulated wave
 */
class WaveParticle : public Poolable
{
private:
    Vector2 m_position;
    Vector2 m_velocity;
    REAL m_amplitude;
    REAL m_radius;

protected:
    void onAlloc();
    void onFree();

public:
    WaveParticle();
    ~WaveParticle();

    Vector2& position();
    void setPosition(const Vector2 &);

    Vector2& velocity();
    void setVelocity(const Vector2 &);

    REAL amplitude();
    void setAmplitude(REAL);

    REAL radius();
    void setRadius(REAL);

    /**
     * Updates this particle
     * @param dt The amount of time that has elapsed since the last update() call, in seconds
     */
    void update(REAL dt);
};

#endif // WAVEPARTICLE_H
