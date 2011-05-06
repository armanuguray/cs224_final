#include "RigidBodySimulation.h"
#include "RigidBodyConstants.h"
#include "RigidBody.h"
#include "RigidBodyRendering.h"
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <iostream>
#include "Camera.h"

RigidBodySimulation::RigidBodySimulation(const QGLContext *context, Camera *camera)
{
    m_camera = camera;

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

    // load shaders
    load_shaders(context);
    load_fbos();
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

    // delete shaders
    for (std::map<std::string, QGLShaderProgram*>::iterator it = m_shaders.begin(); it != m_shaders.end(); ++it)
        delete it->second;

    // delete framebuffers
    delete m_lowresbuffer;
}

void RigidBodySimulation::load_fbos()
{
    m_lowresbuffer = new QGLFramebufferObject(BUOYANCY_IMAGE_RESOLUTION,
                                              BUOYANCY_IMAGE_RESOLUTION,
                                              QGLFramebufferObject::NoAttachment,
                                              GL_TEXTURE_2D,GL_RGB16F_ARB);
}

void RigidBodySimulation::load_shaders(const QGLContext *context)
{
    // load buoyancy
    QGLShaderProgram *shader = new QGLShaderProgram(context);
    if(!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/buoyancy.vert")) {
        std::cerr << "Vertex Shader:\n" << shader->log().data() << std::endl;
        exit(1);
    }
    if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/buoyancy.frag")) {
        std::cerr << "Fragment Shader:\n" << shader->log().data() << std::endl;
        exit(1);
    }
    m_shaders["buoyancy"] = shader;
}

void RigidBodySimulation::stepSimulation()
{
    // apply forces on all objects
    btScalar volume;
    RigidBody *rb;
    for (std::set<RigidBody *>::iterator it = m_rigidbodies.begin(); it != m_rigidbodies.end(); ++it)
    {
        rb = *it;
        if ((volume = rb->computeSubmergedVolume(0, m_lowresbuffer, m_shaders["buoyancy"], m_camera->getWidth(), m_camera->getHeight()) > 0)) { // TODO: pass the heightmap instead of 0
            // TODO: apply buoyancy force if the object is floating (not applying unnecessary forces makes bullet run faster)
        }
        // TODO: apply lift and drag
    }

    // step the bullet physics simulation
    m_dynamics_world->stepSimulation(1);
}

RigidBody* RigidBodySimulation::addRigidBody(RigidBodyType type, btScalar mass, btVector3 &inertia, btTransform &initial_transform)
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
    return rb;
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