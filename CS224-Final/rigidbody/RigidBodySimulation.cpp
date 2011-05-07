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
    m_dynamics_world->setGravity(btVector3(0, -GRAVITY, 0));
//    m_dynamics_world->setGravity(btVector3(0, 0, 0));

    // setup collision shapes
    m_sphere_collisionshape = new btSphereShape(1);
    m_cube_collisionshape = new btBoxShape(btVector3(SIDE_LENGTH/2.0f, SIDE_LENGTH/2.0f, SIDE_LENGTH/2.0f));

    // intialize the rigid body pool
    for (unsigned i = 0; i < RIGIDBODY_MAX_COUNT; i++)
        m_rigidbody_pool.add(new RigidBody());

    // load shaders
    loadShaders(context);
    load_fbos();

    // allocate utility buffers
    m_lowres = new GLfloat[BUOYANCY_IMAGE_RESOLUTION*BUOYANCY_IMAGE_RESOLUTION*3];
    // this is a floor for testing
//    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
//    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
//    btRigidBody::btRigidBodyConstructionInfo
//            groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
//    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
//    m_dynamics_world->addRigidBody(groundRigidBody);
}

RigidBodySimulation::~RigidBodySimulation()
{
    // remove all objects from the simulation (their deletion will be handled by the pool)
    foreach (RigidBody *rb, m_rigidbodies) {
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
    foreach (QString key, m_buffers.keys()) {
        delete m_buffers[key];
    }
    m_buffers.clear();

    delete[] m_lowres;
}

void RigidBodySimulation::load_fbos()
{
    QGLFramebufferObject *lowresbuffer; // the resolution of this buffer is BUOYANCY_IMAGE_RESOLUTION
    lowresbuffer = new QGLFramebufferObject(BUOYANCY_IMAGE_RESOLUTION,
                                            BUOYANCY_IMAGE_RESOLUTION,
                                            QGLFramebufferObject::NoAttachment,
                                            GL_TEXTURE_2D, GL_RGB16F_ARB);

    m_buffers["low-res"] = lowresbuffer;

    lowresbuffer = new QGLFramebufferObject(BUOYANCY_IMAGE_RESOLUTION,
                                            BUOYANCY_IMAGE_RESOLUTION,
                                            QGLFramebufferObject::NoAttachment,
                                            GL_TEXTURE_2D, GL_RGB16F_ARB);
    m_buffers["low-res2"] = lowresbuffer;
}

void RigidBodySimulation::loadShader(const QGLContext *context, const QString &name)
{
    QString vertex_name = ":/" + name + ".vert";
    QString frag_name = ":/" + name + ".frag";

    QGLShaderProgram *shader = new QGLShaderProgram(context);
    if (!shader->addShaderFromSourceFile(QGLShader::Vertex, vertex_name)) {
        std::cerr << "Vertex Shader:\n" << shader->log().data() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!shader->addShaderFromSourceFile(QGLShader::Fragment, frag_name)) {
        std::cerr << "Fragment Shader:\n" << shader->log().data() << std::endl;
        exit(EXIT_FAILURE);
    }

    m_shaders[name.toStdString()] = shader;
}

void RigidBodySimulation::loadShaders(const QGLContext *context)
{
    loadShader(context, "buoyancy");
    loadShader(context, "wavegen");
    loadShader(context, "waveeffect");
}

void RigidBodySimulation::stepSimulation(float time_elapsed)
{
    // apply forces on all objects
    btScalar volume;
    btVector3 out_centroid;
    foreach (RigidBody *rb, m_rigidbodies)
    {
        if ((volume = rb->computeSubmergedVolume(0, m_buffers["low-res"], m_shaders["buoyancy"], m_camera->getWidth(), m_camera->getHeight(), m_lowres, out_centroid) > 0))
        {
            rb->applyBuoyancy(volume, out_centroid);
        }

        // TODO: apply lift and drag
    }

    // step the bullet physics simulation
    m_dynamics_world->stepSimulation(time_elapsed, 10);
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
    foreach (RigidBody *rb, m_rigidbodies) {
        rb->render();
    }
}

void RigidBodySimulation::generateWaves(WaveParticleManager &manager) {
    foreach (RigidBody *rb, m_rigidbodies) {
        rb->generateWaves(manager, m_shaders, m_buffers, m_lowres, m_camera->getWidth(), m_camera->getHeight());
    }
}
