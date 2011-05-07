/**
 * @author auguray
 * @paragraph The physics simulation is managed by this class, using Bullet Physics
 */
#ifndef RIGIDBODYSIMULATION_H
#define RIGIDBODYSIMULATION_H

#include "Pool.h"
#include <btBulletDynamicsCommon.h>
#include <QSet>
#include <map>
#include <string>
#include <QString>
#include <QMap>
#include "OpenGLInclude.h"
#include "WaveParticleManager.h"

class QGLContext;
class QGLFramebufferObject;
class QGLShaderProgram;
class RigidBody;
class Camera;

typedef enum {
    RigidBodyTypeSphere,
    RigidBodyTypeCube,
    RigidBodyTypeBoat
} RigidBodyType;

class RigidBodySimulation
{
public:
    RigidBodySimulation(const QGLContext *context, Camera *camera);
    ~RigidBodySimulation();

    // steps the simulation
    void stepSimulation(float time_elapsed);

    // generates waves
    void generateWaves(WaveParticleManager &manager);

    // renders all rigid bodies that are currently alive
    void renderAll();

    // adds a rigidbody to the simulation and returns a pointer to it
    RigidBody* addRigidBody(RigidBodyType type, btScalar mass, btVector3 &inertia, btTransform &initial_transform);
    void removeRigidBody(RigidBody *body);

private:

    // pool of rigidbodies allocated on startup
    Pool m_rigidbody_pool;
    // set of current bodies that are in the simulation
    QSet<RigidBody *> m_rigidbodies;

    /* dynamic world simulation */
    btBroadphaseInterface *m_broadphase;
    btDefaultCollisionConfiguration *m_collision_configuration;
    btCollisionDispatcher *m_dispatcher;
    btSequentialImpulseConstraintSolver *m_solver;
    btDiscreteDynamicsWorld *m_dynamics_world;

    /* Collision shapes that are shared among resources */
    btCollisionShape *m_sphere_collisionshape;
    btCollisionShape *m_cube_collisionshape;

    /* Shader programs used in force calculations */
    void load_shaders(const QGLContext *context);
    std::map<std::string, QGLShaderProgram*> m_shaders;

    /* frame buffers */
    void load_fbos();
    QMap<QString, QGLFramebufferObject *> m_buffers;
    GLfloat *m_lowres; // low resolution buffer to store the contents of m_lowresbuffer on the CPU

    // the current camera
    Camera *m_camera;
};

#endif // RIGIDBODYSIMULATION_H
