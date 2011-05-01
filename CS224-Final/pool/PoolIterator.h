#ifndef POOLITERATOR_H
#define POOLITERATOR_H

#include <QSet>
#include "Pool.h"

/**
 * Iterates through a Pool
 * @author Dave
 */
class PoolIterator
{
private:
    /** The iterator into the Pool's Poolable list */
    QSetIterator<Poolable *> m_it;
    /** The next live Poolable found */
    Poolable *m_next;

    /** Walks m_it to the next live poolable and stores it in m_next. If none was found, sets m_next to NULL */
    void walk();

public:
    /** Creates an iterator that iterates through the Poolables in the given Pool */
    PoolIterator(Pool *pool);
    /** Releases resources in use by this iterator */
    ~PoolIterator();

    /** Gets a value indicating whether there is another object to iterate through */
    bool hasNext();
    /** Gets the next item from this iterator and advances this iterator */
    Poolable *next();
    /** Gets the next item from this iterator without advancing this iterator */
    Poolable *peek();
};

#endif // POOLITERATOR_H
