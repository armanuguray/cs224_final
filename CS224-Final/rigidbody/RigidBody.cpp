#include "RigidBody.h"

RigidBody::RigidBody()
{
    m_internal_rigidbody = NULL;
    m_internal_collisionshape = NULL;
    m_internal_defaultmotionstate = NULL;
    m_render_function = NULL;
}

RigidBody::~RigidBody()
{
    delete m_internal_rigidbody;
    delete m_internal_defaultmotionstate;
}

void RigidBody::onAlloc()
{

}

void RigidBody::onFree()
{
    delete m_internal_rigidbody;
    m_internal_rigidbody = NULL;
    delete m_internal_defaultmotionstate;
    m_internal_defaultmotionstate = NULL;
    m_render_function = NULL;
}

void RigidBody::initialize(btScalar mass, btVector3 &inertia, const btTransform &initial_transform, btCollisionShape *collision_shape, void (*render_function)())
{
    m_internal_defaultmotionstate = new btDefaultMotionState(initial_transform);
    collision_shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo ci(mass, m_internal_defaultmotionstate, collision_shape, inertia);
    m_internal_rigidbody = new btRigidBody(ci);
    m_render_function = render_function;
}

void RigidBody::render()
{
    if (m_render_function != NULL)
        m_render_function();
}
