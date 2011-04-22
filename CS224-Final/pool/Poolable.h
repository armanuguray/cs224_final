#ifndef POOLABLE_H
#define POOLABLE_H

/**
 * Base class for an object that can be placed in a Pool for reuse
 * @author Dave
 */
class Poolable
{
private:
    /** Whether or not this object is 'valid' */
    bool m_alive;

protected:
    /**
      * Called by Poolable::alloc() before this particle is marked as alive.
      * Places the newly reactivated object in a consistent state, like a constructor.
      */
    virtual void onAlloc() = 0;

    /**
     * Called by Poolable::fre() before after this particle is marked as dead.
     * Releases any resources in use by the object, like a destructor
     */
    virtual void onFree() = 0;

    /** Sets the value returned by Poolable::isAlive() */
    void setAlive(bool);

public:
    /** Creates a new Poolable */
    Poolable();

    /** Gets a value indicating whether this Poolable is currently in use */
    bool isAlive();

    /** Allocates this object if it isn't already allocated. No-op if this object is already alive (see isAlive()) */
    void alloc();

    /** Deallocated this object if it's currently allocated. No-op if this object isn't alive (see isAlive()) */
    void free();
};

#endif // POOLABLE_H
