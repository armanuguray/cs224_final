#include "Pool.h"

#include <btBulletCollisionCommon.h>

Pool::Pool()
{
}

Pool::~Pool()
{
    clear();
}

bool Pool::hasLive() const
{
    return liveCount() > 0;
}

bool Pool::hasDead() const
{
    return deadCount() > 0;
}

int Pool::capacity() const
{
    return liveCount() + deadCount();
}

int Pool::liveCount() const
{
    return m_allocated.size();
}

int Pool::deadCount() const
{
    return m_unallocated.size();
}

void Pool::add(Poolable *p)
{
    m_unallocated.insert(p);
}

void Pool::remove(bool removeLive)
{
    if (!m_unallocated.empty()) {
        Poolable *toRemove = *m_unallocated.end();
        m_allocated.remove(toRemove);
        delete toRemove;
        return;
    }

    // every object is allocated; we must remove an allocated one!
    if (removeLive) {
        Poolable *toRemove = *m_allocated.end();
        m_allocated.remove(toRemove);
        delete toRemove;
        return;
    }
}

void Pool::clear()
{
    foreach (Poolable *poolable, m_allocated) {
        poolable->free();
        delete poolable;
    }

    foreach (Poolable *poolable, m_unallocated) {
        poolable->free();
        delete poolable;
    }

    m_allocated.clear();
    m_unallocated.clear();
}

Poolable *Pool::alloc()
{
    if (m_unallocated.size() == 0) {
        return NULL;
    }

    Poolable *toAlloc = *m_unallocated.begin();
    m_unallocated.remove(toAlloc);
    m_allocated.insert(toAlloc);
    toAlloc->alloc();
    return toAlloc;
}

void Pool::free(Poolable *poolable) {
    poolable->free();
    m_allocated.remove(poolable);
    m_unallocated.insert(poolable);
}
