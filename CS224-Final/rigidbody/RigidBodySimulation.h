/**
 * @author auguray
 * @paragraph The physics simulation is managed by this class, using Bullet Physics
 */
#ifndef RIGIDBODYSIMULATION_H
#define RIGIDBODYSIMULATION_H

#include "Pool.h"
#include <btBulletDynamicsCommon.h>
#include <set>

class RigidBody;

typedef enum {
    RigidBodyTypeSphere,
    RigidBodyTypeCube,
    RigidBodyTypeBoat
} RigidBodyType;

class RigidBodySimulation
{
public:
    RigidBodySimulation();
    ~RigidBodySimulation();

    // steps the simulation by the given time
    void stepSimulation(double seconds);

    // renders all rigid bodies that are currently alive
    void renderAll();

    void addRigidBody(RigidBodyType type, btScalar mass, btVector3 &inertia, btTransform &initial_transform);
    void removeRigidBody(RigidBody *body);

private:

    // pool of rigidbodies allocated on startup
    Pool m_rigidbody_pool;
    // set of current bodies that are in the simulation
    std::set<RigidBody *> m_rigidbodies;

    /* dynamic world simulation */
    btBroadphaseInterface *m_broadphase;
    btDefaultCollisionConfiguration *m_collision_configuration;
    btCollisionDispatcher *m_dispatcher;
    btSequentialImpulseConstraintSolver *m_solver;
    btDiscreteDynamicsWorld *m_dynamics_world;

    /* Collision shapes that are shared among resources */
    btCollisionShape *m_sphere_collisionshape;
    btCollisionShape *m_cube_collisionshape;
};

#endif // RIGIDBODYSIMULATION_H
