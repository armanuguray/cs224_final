
#include "WaveConstants.h"
#include "WaveParticle.h"

WaveParticle::WaveParticle() : m_position(0, 0),
                               m_velocity(0, 0),
                               m_amplitude(0),
                               m_radius(0),
                               m_dispersionOrigin(0, 0),
                               m_dispersionAngle(0)
{
}

WaveParticle::WaveParticle(const Vector2 &position, const Vector2 &velocity, REAL amplitude, REAL radius, const Vector2 &dispersionOrigin, REAL dispersionAngle)
                         : m_position(position),
                           m_velocity(velocity),
                           m_amplitude(amplitude),
                           m_radius(radius),
                           m_dispersionOrigin(dispersionOrigin),
                           m_dispersionAngle(dispersionAngle)
{
}

WaveParticle::WaveParticle(REAL amplitude, REAL radius, const Vector2 &dispersionOrigin, REAL dispersionAngle, REAL movementAngle)
                         : m_position(dispersionOrigin),
                           m_velocity(WAVE_SPEED * cos(movementAngle), WAVE_SPEED * sin(movementAngle)),
                           m_amplitude(amplitude),
                           m_radius(radius),
                           m_dispersionOrigin(dispersionOrigin),
                           m_dispersionAngle(dispersionAngle)
{
}

WaveParticle::~WaveParticle()
{
}

REAL WaveParticle::sign(REAL a)
{
    return a < 0 ? -1.0 : 1.0;
}

void WaveParticle::onAlloc()
{
}

void WaveParticle::onFree()
{
}

void WaveParticle::spawn(const Vector2 &position, const Vector2 &velocity, REAL amplitude, REAL radius, const Vector2 &dispersionOrigin, REAL dispersionAngle)
{
    m_position = position;
    m_velocity = velocity;
    m_amplitude = amplitude;
    m_radius = radius;
    m_dispersionOrigin = dispersionOrigin;
    m_dispersionAngle = dispersionAngle;
}

void WaveParticle::spawn(REAL amplitude, REAL radius, const Vector2 &dispersionOrigin, REAL dispersionAngle, REAL movementAngle)
{
    m_position = dispersionOrigin;
    m_velocity = Vector2(WAVE_SPEED * cos(movementAngle), WAVE_SPEED * sin(movementAngle));
    m_amplitude = amplitude;
    m_radius = radius;
    m_dispersionOrigin = dispersionOrigin;
    m_dispersionAngle = dispersionAngle;
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

Vector2& WaveParticle::dispersionOrigin()
{
    return m_dispersionOrigin;
}

void WaveParticle::setDispersionOrigin(const Vector2 &v)
{
    m_dispersionOrigin = v;
}

REAL WaveParticle::dispersionAngle()
{
    return m_dispersionAngle;
}

void WaveParticle::setDispersionAngle(REAL r)
{
    m_dispersionAngle = r;
}

void WaveParticle::update(QLinkedList<WaveParticle *> *liveParticles, Pool *particles, REAL dt)
{
    // Move
    m_position += m_velocity * dt;

    // Decrease this particle's amplitude
    m_amplitude += WAVE_AMPLITUDE_FALLOFF * dt * -sign(m_amplitude);

    // Die if the amplitude is too low
    if (fabs(m_amplitude) < WAVE_MIN_AMPLITUDE)
    {
        liveParticles->removeAll(this);
        free();
    }

    // Subdivide if necessary
    REAL maxDist = m_radius * WAVE_SUBDIVISION_COEFFICIENT;
    Vector2 fromCenter = m_position - m_dispersionOrigin;
    REAL dist = fromCenter.getMagnitude() * m_dispersionAngle;
    if (dist > maxDist)
    {
        // Find the current angle from the origin
        float theta = atan2(fromCenter.y, fromCenter.x);

        // Use the bisections between this and neighboring particles to find their angles
        float thetaLeft  = theta - .5 * m_dispersionAngle;
        float thetaRight = theta + .5 * m_dispersionAngle;

        // Compute the new amplitude and dispersion angle
        m_dispersionAngle *= .5;
        // TODO: how to compute the new amplitude? Just using a simple average for now, but I'm not sure that's right
        m_amplitude *= .25;

        // Create the particles
        WaveParticle *here = (WaveParticle *)particles->alloc();
        WaveParticle *left = (WaveParticle *)particles->alloc();
        WaveParticle *right = (WaveParticle *)particles->alloc();

        here->setAmplitude(m_amplitude);
        here->setDispersionAngle(m_dispersionAngle);
        here->setDispersionOrigin(m_dispersionOrigin);
        here->setRadius(m_radius);
        here->setPosition(m_position);
        here->setVelocity(m_velocity);

        left->setAmplitude(m_amplitude);
        left->setDispersionAngle(m_dispersionAngle);
        left->setDispersionOrigin(m_dispersionOrigin);
        left->setRadius(m_radius);

        Vector2 off = m_position - m_dispersionOrigin;
        float dist = off.getMagnitude();
        float theta0 = atan2(off.y, off.x);

        theta = theta0 + thetaLeft;
        Vector2 dir(cos(theta), sin(theta));
        left->setPosition(m_dispersionOrigin + dir * dist);
        left->setVelocity(dir * m_velocity.getMagnitude());

        right->setAmplitude(m_amplitude);
        right->setDispersionAngle(m_dispersionAngle);
        right->setDispersionOrigin(m_dispersionOrigin);
        right->setRadius(m_radius);

        theta = theta0 + thetaRight;
        dir.x = cos(theta);
        dir.y = sin(theta);
        right->setPosition(m_dispersionOrigin + dir * dist);
        right->setVelocity(dir * m_velocity.getMagnitude());

        liveParticles->append(here);
        liveParticles->append(left);
        liveParticles->append(right);
    }
}
