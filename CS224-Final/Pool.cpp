
#include "Pool.h"

Pool::Pool() {}

Pool::~Pool()
{
    clear();
}

bool Pool::hasLive() const
{
    Poolable *p = 0;
    for (QLinkedListIterator<Poolable*> it(m_data); it.hasNext(); p = it.next())
        if (p->isAlive())
            return true;
    return false;
}

bool Pool::hasDead() const
{
    Poolable *p = 0;
    for (QLinkedListIterator<Poolable*> it(m_data); it.hasNext(); p = it.next())
        if (!p->isAlive())
            return true;
    return false;
}

int Pool::capacity() const
{
    int c = 0;
    Poolable *p = 0;
    for (QLinkedListIterator<Poolable*> it(m_data); it.hasNext(); p = it.next())
        ++c;
    return c;
}

int Pool::liveCount() const
{
    int c = 0;
    Poolable *p = 0;
    for (QLinkedListIterator<Poolable*> it(m_data); it.hasNext(); p = it.next())
        if (p->isAlive())
            ++c;
    return c;
}

int Pool::deadCount() const
{
    int c = 0;
    Poolable *p = 0;
    for (QLinkedListIterator<Poolable*> it(m_data); it.hasNext(); p = it.next())
        if (p->isAlive())
            ++c;
    return c;
}

void Pool::add(Poolable *p)
{
    m_data.append(p);
}

void Pool::remove(bool removeLive)
{
    Poolable *live = 0, *dead = 0, *p = 0;
    for (QLinkedListIterator<Poolable*> it(m_data); it.hasNext(); p = it.next())
    {
        if (p->isAlive())
        {
            live = p;
        }
        else
        {
            dead = p;
            break;
        }
    }

    if (dead)
    {
        m_data.removeAll(dead);
        delete dead;
    }
    else if (removeLive)
    {
        m_data.removeAll(live);
        delete live;
    }
}

void Pool::clear()
{
    Poolable *p = 0;
    for (QLinkedListIterator<Poolable*> it(m_data); it.hasNext(); p = it.next())
        delete p;
    m_data.clear();
}

Poolable *Pool::alloc()
{
    Poolable *free = 0;
    Poolable *p = 0;
    for (QLinkedListIterator<Poolable*> it(m_data); it.hasNext(); p = it.next())
    {
        if (p->isAlive())
        {
            free = p;
            break;
        }
    }

    if (free)
        free->alloc();

    return free;
}
