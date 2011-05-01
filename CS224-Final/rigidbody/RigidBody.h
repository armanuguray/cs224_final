/**
 * @author auguray
 * @paragraph This represent a rigid body which is part of the physics simulation
 */

#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Poolable.h"

class RigidBody : public Poolable
{
public:
    RigidBody();
    virtual ~RigidBody();

    virtual void render() = 0;

protected:
    void onAlloc();
    void onFree();
};

#endif // RIGIDBODY_H
