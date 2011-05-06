
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

void WaveParticle::update(QSet<WaveParticle *> *liveParticles, Pool *particles, REAL dt)
{
    // Move
    m_position += m_velocity * dt;

    // Decrease this particle's amplitude
    m_amplitude += WAVE_AMPLITUDE_FALLOFF * dt * -sign(m_amplitude);

    // Die if the amplitude is too low
    if (fabs(m_amplitude) < WAVE_MIN_AMPLITUDE || fabs(m_position.x) > .5 * WAVE_HEIGHTMAP_WIDTH || fabs(m_position.y) > .5 * WAVE_HEIGHTMAP_HEIGHT)
    {
        liveParticles->remove(this);
        particles->free(this);
    }

    // Subdivide if necessary
    REAL maxDist = m_radius * WAVE_SUBDIVISION_COEFFICIENT;
    Vector2 fromCenter = m_position - m_dispersionOrigin;
//    REAL dist = fromCenter.getMagnitude() * m_dispersionAngle;
    REAL dist = 2.0 * fromCenter.getMagnitude() * tan(m_dispersionAngle * .5);
    if (dist > maxDist)
    {
        // Find the current angle from the origin
        float theta = atan2(fromCenter.y, fromCenter.x);

        // Use the bisections between this and neighboring particles to find their angles
        float thetaLeft  = theta - .3333 * m_dispersionAngle;
        float thetaRight = theta + .3333 * m_dispersionAngle;

        // Compute the new amplitude and dispersion angle
        m_dispersionAngle *= .3333;
        m_amplitude *= .3333;

        // Create the particles; do nothing if we're out of particles
        WaveParticle *left = (WaveParticle *)particles->alloc();
        if (left == NULL) {
            return;
        }

        WaveParticle *right = (WaveParticle *)particles->alloc();
        if (right == NULL) {
            particles->free(left);
            return;
        }

        left->setAmplitude(m_amplitude);
        left->setDispersionAngle(m_dispersionAngle);
        left->setDispersionOrigin(m_dispersionOrigin);
        left->setRadius(m_radius);

        Vector2 off = m_position - m_dispersionOrigin;
        float dist = off.getMagnitude();

        theta = thetaLeft;
        Vector2 dir(cos(theta), sin(theta));
        left->setPosition(m_dispersionOrigin + dir * dist);
        left->setVelocity(dir * m_velocity.getMagnitude());

        right->setAmplitude(m_amplitude);
        right->setDispersionAngle(m_dispersionAngle);
        right->setDispersionOrigin(m_dispersionOrigin);
        right->setRadius(m_radius);

        theta = thetaRight;
        dir.x = cos(theta);
        dir.y = sin(theta);
        right->setPosition(m_dispersionOrigin + dir * dist);
        right->setVelocity(dir * m_velocity.getMagnitude());

        liveParticles->insert(left);
        liveParticles->insert(right);
    }
}
