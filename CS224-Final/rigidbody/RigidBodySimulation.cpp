#include "RigidBodySimulation.h"
#include "RigidBodyConstants.h"
#include "RigidBody.h"
#include "RigidBodyRendering.h"
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <iostream>
#include "Camera.h"

RigidBodySimulation::RigidBodySimulation(const QGLContext *context, Camera *camera, WaveParticleManager *waveparticlemanager)
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
        m_rigidbody_pool.add(new RigidBody(m_camera, waveparticlemanager));

    // load shaders
    loadShaders(context);
    loadFramebufferObjects();

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
    foreach (QString key, m_shaders.keys()) {
        delete m_shaders[key];
    }
    m_shaders.clear();

    // delete framebuffers
    foreach (QString key, m_buffers.keys()) {
        delete m_buffers[key];
    }
    m_buffers.clear();

    delete[] m_lowres;
}

void RigidBodySimulation::loadFramebufferObject(int size, const QString &name, bool alpha)
{
    QGLFramebufferObject *buffer;

    if (!alpha) {
        buffer = new QGLFramebufferObject(size, size,
                                          QGLFramebufferObject::NoAttachment,
                                          GL_TEXTURE_2D, GL_RGB16F_ARB);
    } else {
        buffer = new QGLFramebufferObject(size, size,
                                          QGLFramebufferObject::NoAttachment,
                                          GL_TEXTURE_2D, GL_RGBA16F_ARB);
    }


    m_buffers[name] = buffer;
}

void RigidBodySimulation::loadFramebufferObjects()
{
    loadFramebufferObject(BUOYANCY_IMAGE_RESOLUTION, "low-res");
    loadFramebufferObject(BUOYANCY_IMAGE_RESOLUTION, "low-res2", true);
    loadFramebufferObject(BUOYANCY_IMAGE_RESOLUTION, "low-res3", true);

    loadFramebufferObject(8, "8x8", true);
    loadFramebufferObject(4, "4x4", true);
    loadFramebufferObject(2, "2x2", true);
    loadFramebufferObject(1, "1x1", true);

    loadFramebufferObject(8, "8x8 2", true);
    loadFramebufferObject(4, "4x4 2", true);
    loadFramebufferObject(2, "2x2 2", true);
    loadFramebufferObject(1, "1x1 2", true);
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
    m_shaders[name] = shader;
}

void RigidBodySimulation::loadShaders(const QGLContext *context)
{
    logln("buoyancy");
    loadShader(context, "buoyancy");
    logln("wavegen");
    loadShader(context, "wavegen");
    logln("waveeffect");
    loadShader(context, "waveeffect");
    logln("computedir");
    loadShader(context, "computedir");
    logln("downscale");
    loadShader(context, "downscale");
    logln("upscale");
    loadShader(context, "upscale");
    logln("liftdrag");
    loadShader(context, "liftdrag");
}

void RigidBodySimulation::stepSimulation(float time_elapsed)
{
    // apply forces on all objects
    btScalar volume;
    btVector3 out_centroid;
    foreach (RigidBody *rb, m_rigidbodies)
    {
        if ((volume = rb->computeSubmergedVolume(0, m_buffers["low-res"], m_shaders["buoyancy"], m_camera->getWidth(), m_camera->getHeight(), m_lowres, out_centroid)) > 0)
        {
            rb->applyBuoyancy(volume, out_centroid);
            rb->applyLiftAndDrag(0, m_buffers["low-res"], m_shaders["liftdrag"], m_camera->getWidth(), m_camera->getHeight(), m_lowres);
        }
    }

    // step the bullet physics simulation
    m_dynamics_world->stepSimulation(time_elapsed, MAX_SUBSTEPS);
}

RigidBody* RigidBodySimulation::addRigidBody(RigidBodyType type, btScalar mass, btVector3 &inertia, btTransform &initial_transform)
{
    void (*render_func)();
    btCollisionShape *cs;
    /*
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
    */

    cs = m_cube_collisionshape;                   // Sorry, we only have boxes. Honestly, we don't care.
    render_func = &RigidBodyRendering::renderBox; //

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
