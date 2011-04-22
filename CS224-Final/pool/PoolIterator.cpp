
#include "PoolIterator.h"

PoolIterator::PoolIterator(Pool *pool) : m_it(pool->m_data), m_next(0)
{
    walk();
}

PoolIterator::~PoolIterator() {}

void PoolIterator::walk()
{
    m_next = 0;
    while (!m_next && m_it.hasNext())
    {
        m_next = m_it.next();
        if (!m_next->isAlive())
            m_next = 0;
    }
}

bool PoolIterator::hasNext()
{
    return m_next != 0;
}

Poolable* PoolIterator::next()
{
    Poolable *p = m_next;
    walk();
    return p;
}

Poolable* PoolIterator::peek()
{
    return m_next;
}
