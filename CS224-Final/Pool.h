#ifndef POOL_H
#define POOL_H

#include <QLinkedList>
#include "Poolable.h"

/**
 * Groups a collection of objects
 * @author Dave
 *
 * Pool mypool;
 * for (int i = 0; i < MY_POOL_CAPCITY; ++i)
 *     mypool.add(new MyThing());
 *
 * ...
 *
 * MyThing *athing = (MyThing*)mypool.alloc();
 *
 * ...
 *
 * PoolIterator it(&mypool);
 * while (it.hasNext())
 * {
 *     MyThing *thing = (MyThing*)it.next();
 *
 *     ...
 * }
 */
class Pool
{
private:
    /** All objects in this pool, both allocated and unallocated. */
    QLinkedList<Poolable*> m_data;

    friend class PoolIterator;

public:
    /** Creates a pool with an initial capacity of zero */
    Pool();
    /** Unallocates all objects in this pool. */
    ~Pool();

    /** Gets a value indicating whether this pool has any allocated objects */
    bool hasLive();
    /** Gets a value indicating whether this pool has any free objects */
    bool hasDead();

    /** Gets the total number of objects that can be allocated at once in this pool */
    int capacity();
    /** Gets the number of allocated objects in this pool */
    int liveCount();
    /** Gets the number of free objects in this pool */
    int deadCount();

    /** Adds the given poolable to the pool. This operation increases the capacity of this pool by one */
    void add(Poolable *p);
    /**
     * Attempts to remove a poolable from this pool, decreasing the capacity of this pool by one.
     * If there is at least one free poolable, it will be removed; otherwise, an allocated poolable will be
     * removed if true is passed for removeLive. Any objects removed from the pool are immediately deleted.
     * @param removeLive Whether or not this method is allowed to remove a currently allocated Poolable if needed
     */
    void remove(bool removeLive = false);
    /** Deletes all objects in this pool and sets its capacity to zero */
    void clear();

    /**
      * Recycles a free object in the pool (by calling alloc()) and returns it. If there are
      * no more free objects in the pool, NULL is returned instead
      */
    Poolable* alloc();
};

#endif // POOL_H
