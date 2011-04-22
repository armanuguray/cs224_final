
#include "Wave.h"

Wave::Wave() {}

Wave::~Wave() {}

QList<WaveParticle*>& Wave::particles()
{
    return m_particles;
}

void Wave::update(Pool *particles, REAL dt)
{
    QListIterator<WaveParticle*> it(m_particles);
    while (it.hasNext())
    {
        WaveParticle *particle = it.next();
        particle->update(dt);
    }
}
