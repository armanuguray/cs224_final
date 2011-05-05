#include "RigidBodySimulation.h"
#include "RigidBodyConstants.h"
#include "RigidBody.h"
#include "RigidBodyRendering.h"

RigidBodySimulation::RigidBodySimulation()
{
    // setup rigidbody rendering utilities
    RigidBodyRendering::initialize();

    // setup the dynamics world
    m_broadphase = new btDbvtBroadphase();
    m_collision_configuration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collision_configuration);
    m_solver = new btSequentialImpulseConstraintSolver();
    m_dynamics_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collision_configuration);
    m_dynamics_world->setGravity(btVector3(0, -10, 0));

    // setup collision shapes
    m_sphere_collisionshape = new btSphereShape(1);
    m_cube_collisionshape = new btBoxShape(btVector3(0.5, 0.5, 0.5));

    // intialize the rigid body pool
    for (unsigned i = 0; i < RIGIDBODY_MAX_COUNT; i++)
        m_rigidbody_pool.add(new RigidBody());
}

RigidBodySimulation::~RigidBodySimulation()
{
    // remove all objects from the simulation (their deletion will be handled by the pool)
    RigidBody *rb;
    m_rigidbodies.clear();
    for (std::set<RigidBody *>::iterator it = m_rigidbodies.begin(); it != m_rigidbodies.end(); ++ it) {
        rb = (*it);
        m_dynamics_world->removeRigidBody(rb->getInternalRigidBody());
    }
    m_rigidbody_pool.clear();

    // delete collision shapes
    delete m_sphere_collisionshape;
    delete m_cube_collisionshape;

    // delete dynamics world
    delete m_dynamics_world;
    delete m_solver;
    delete m_dispatcher;
    delete m_collision_configuration;
    delete m_broadphase;

    // cleanup rigidbody rendering resources
    RigidBodyRendering::cleanup();
}

void RigidBodySimulation::stepSimulation(double seconds)
{
    m_dynamics_world->stepSimulation(seconds);
}

void RigidBodySimulation::addRigidBody(RigidBodyType type, btScalar mass, btVector3 &inertia, btTransform &initial_transform)
{
    void (*render_func)();
    btCollisionShape *cs;
    switch (type)
    {
    case RigidBodyTypeSphere:
        cs = m_sphere_collisionshape;
        render_func = &RigidBodyRendering::renderSphere;
        break;
    case RigidBodyTypeCube:
        cs = m_cube_collisionshape;
        render_func = &RigidBodyRendering::renderBox;
        break;
    // TODO: handle different types (for example boat)
    default:
        break;
    }
    RigidBody *rb = (RigidBody *)m_rigidbody_pool.alloc();
    rb->initialize(mass, inertia, initial_transform, cs, render_func);
    m_dynamics_world->addRigidBody(rb->getInternalRigidBody());
    m_rigidbodies.insert(rb);
}

void RigidBodySimulation::removeRigidBody(RigidBody *body)
{
    m_dynamics_world->removeRigidBody(body->getInternalRigidBody());
    m_rigidbodies.erase(m_rigidbodies.find(body));
    m_rigidbody_pool.free(body);
}

void RigidBodySimulation::renderAll()
{
    RigidBody *rb;
    for (std::set<RigidBody *>::iterator it = m_rigidbodies.begin(); it != m_rigidbodies.end(); ++ it) {
        rb = (*it);
        rb->render();
    }
}
