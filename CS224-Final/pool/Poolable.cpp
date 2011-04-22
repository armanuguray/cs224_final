
#include "Poolable.h"

Poolable::Poolable()
{
    m_alive = false;
}

void Poolable::setAlive(bool a)
{
    m_alive = a;
}

bool Poolable::isAlive()
{
    return m_alive;
}

void Poolable::alloc()
{
    if (!isAlive())
    {
        onAlloc();
        setAlive(true);
    }
}

void Poolable::free()
{
    if (isAlive())
    {
        setAlive(false);
        onFree();
    }
}
