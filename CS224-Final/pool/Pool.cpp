
#include "Pool.h"

Pool::Pool() {}

Pool::~Pool()
{
    clear();
}

bool Pool::hasLive() const
{
    QLinkedListIterator<Poolable*> it(m_data);
    while (it.hasNext())
        if (it.next()->isAlive())
            return true;
    return false;
}

bool Pool::hasDead() const
{
    QLinkedListIterator<Poolable*> it(m_data);
    while (it.hasNext())
        if (!it.next()->isAlive())
            return true;
    return false;
}

int Pool::capacity() const
{
    return m_data.count();
}

int Pool::liveCount() const
{
    int c = 0;
    QLinkedListIterator<Poolable*> it(m_data);
    while (it.hasNext())
        if (it.next()->isAlive())
            ++c;
    return c;
}

int Pool::deadCount() const
{
    int c = 0;
    QLinkedListIterator<Poolable*> it(m_data);
    while (it.hasNext())
        if (!it.next()->isAlive())
            ++c;
    return c;
}

void Pool::add(Poolable *p)
{
    m_data.append(p);
}

void Pool::remove(bool removeLive)
{
    Poolable *live = 0, *dead = 0;
    QLinkedListIterator<Poolable*> it(m_data);
    while (it.hasNext())
    {
        Poolable *p = it.next();
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
    QLinkedListIterator<Poolable*> it(m_data);
    while (it.hasNext())
        delete it.next();
    m_data.clear();
}

Poolable *Pool::alloc()
{
    Poolable *free = 0;
    QLinkedListIterator<Poolable*> it(m_data);
    while (it.hasNext())
    {
        Poolable *p = it.next();
        if (!p->isAlive())
        {
            free = p;
            break;
        }
    }

    if (free)
        free->alloc();

    return free;
}
