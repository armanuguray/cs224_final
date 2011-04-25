#ifndef WAVEPARTICLE_H
#define WAVEPARTICLE_H

#include <QLinkedList>
#include "CS123Algebra.h"
#include "Pool.h"
#include "Poolable.h"

/**
 * A single particle in a simulated wave
 */
class WaveParticle : public Poolable
{
private:
    Vector2 m_position;             // The position of the particle in world space, with Z = the base plane of the projected grid
    Vector2 m_velocity;             // The velocity of the particle, in world coordinates per second, along the projected grid
    REAL m_amplitude;               // The amplitude created at the center of this particle in the heightmap
    REAL m_radius;                  // The radius of influence of this particle
    Vector2 m_dispersionOrigin;     // The point at which the particle was created. Used to calculate when to subdivide
    REAL m_dispersionAngle;         // The angle between this particle and each of its neighbors. Used to calculate when to subdivide

    REAL sign(REAL);

protected:
    void onAlloc();
    void onFree();

public:
    WaveParticle();
    WaveParticle(const Vector2& position, const Vector2& velocity, REAL amplitude, REAL radius, const Vector2& dispersionOrigin, REAL dispersionAngle);
    WaveParticle(REAL amplitude, REAL radius, const Vector2& dispersionOrigin, REAL dispersionAngle, REAL movementAngle);
    ~WaveParticle();

    void spawn(const Vector2& position, const Vector2& velocity, REAL amplitude, REAL radius, const Vector2& dispersionOrigin, REAL dispersionAngle);
    void spawn(REAL amplitude, REAL radius, const Vector2& dispersionOrigin, REAL dispersionAngle, REAL movementAngle);

    Vector2& position();
    void setPosition(const Vector2 &);

    Vector2& velocity();
    void setVelocity(const Vector2 &);

    REAL amplitude();
    void setAmplitude(REAL);

    REAL radius();
    void setRadius(REAL);

    Vector2& dispersionOrigin();
    void setDispersionOrigin(const Vector2 &);

    REAL dispersionAngle();
    void setDispersionAngle(REAL);

    /**
     * Updates this particle
     * @param dt The amount of time that has elapsed since the last update() call, in seconds
     */
    void update(QLinkedList<WaveParticle*> *liveParticles, Pool *particles, REAL dt);
};

#endif // WAVEPARTICLE_H
