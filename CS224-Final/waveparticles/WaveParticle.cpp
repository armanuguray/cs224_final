
#include "WaveConstants.h"
#include "WaveParticle.h"

WaveParticle::WaveParticle() : m_amplitude(0.0),
                               m_dispersionOrigin(0.0, 0.0),
                               m_dispersionAngle(0.0),
                               m_direction(0.0, 0.0),
                               m_time(0.0f)
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

void WaveParticle::spawn(const Vector2& direction, REAL amplitude, float time ,const Vector2& dispersionOrigin, REAL dispersionAngle)
{
    m_direction = direction;
    m_amplitude = amplitude;
    m_time = time;
    m_dispersionOrigin = dispersionOrigin;
    m_dispersionAngle = dispersionAngle;
}

void WaveParticle::spawn(REAL amplitude, const Vector2& dispersionOrigin, REAL dispersionAngle, REAL movementAngle, float time)
{
    m_amplitude = amplitude;
    m_dispersionOrigin = dispersionOrigin;
    m_dispersionAngle = dispersionAngle;
    m_direction.x = cos(movementAngle);
    m_direction.y = sin(movementAngle);
    m_time = time;
}

REAL WaveParticle::amplitude()
{
    return m_amplitude;
}

void WaveParticle::setAmplitude(REAL a)
{
    m_amplitude = a;
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

Vector2& WaveParticle::direction()
{
    return m_direction;
}

void WaveParticle::setDirection(const Vector2 &d)
{
    m_direction = d;
}

float WaveParticle::time()
{
    return m_time;
}

void WaveParticle::setTime(float t)
{
    m_time = t;
}

void WaveParticle::update(QSet<WaveParticle *> *liveParticles, Pool *particles, float t, float dt)
{
    // Subdivide if necessary
    REAL maxDist = WAVE_PARTICLE_RADIUS * WAVE_SUBDIVISION_COEFFICIENT;
    REAL dist = 2.0 * (WAVE_SPEED * (t - m_time)) * tan(m_dispersionAngle * .5);
    if (dist > maxDist)
    {
        // Find the current angle from the origin
        float theta = atan2(m_direction.y, m_direction.x);

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
        left->setDirection(Vector2(cos(thetaLeft), sin(thetaLeft)));
        left->setTime(m_time);

        right->setAmplitude(m_amplitude);
        right->setDispersionAngle(m_dispersionAngle);
        right->setDispersionOrigin(m_dispersionOrigin);
        right->setDirection(Vector2(cos(thetaRight), sin(thetaRight)));
        right->setTime(m_time);

        liveParticles->insert(left);
        liveParticles->insert(right);
    }
}
