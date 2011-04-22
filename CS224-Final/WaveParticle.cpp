
#include "WaveParticle.h"

WaveParticle::WaveParticle() : m_position(0, 0), m_velocity(0, 0), m_amplitude(0), m_radius(0)
{
}

WaveParticle::~WaveParticle()
{
}

void WaveParticle::onAlloc()
{
}

void WaveParticle::onFree()
{
}

Vector2& WaveParticle::position()
{
    return m_position;
}

void WaveParticle::setPosition(const Vector2 &position)
{
    m_position = position;
}

Vector2& WaveParticle::velocity()
{
    return m_velocity;
}

void WaveParticle::setVelocity(const Vector2 &velocity)
{
    m_velocity = velocity;
}

REAL WaveParticle::amplitude()
{
    return m_amplitude;
}

void WaveParticle::setAmplitude(REAL a)
{
    m_amplitude = a;
}

REAL WaveParticle::radius()
{
    return m_radius;
}

void WaveParticle::setRadius(REAL r)
{
    m_radius = r;
}

void WaveParticle::update(REAL dt)
{

}
