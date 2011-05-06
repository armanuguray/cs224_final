/**
 * @author auguray
 * @paragraph This represents a rigid body which is part of the physics simulation
 */

#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Poolable.h"
#include <btBulletDynamicsCommon.h>
#include "OpenGLInclude.h"

class QGLShaderProgram;
class QGLFramebufferObject;

class RigidBody : public Poolable
{
public:
    RigidBody();
    ~RigidBody();

    /* Initialization and rendering */

    // initializes this rigid body
    void initialize(btScalar mass, btVector3 &inertia, const btTransform &initial_transform, btCollisionShape *collision_shape, void (*render_function)());
    // renders the rigidbody using the rendering function provided upon initialization
    void render();

    /* accessors */
    // returns the internal Bullet Physics rigid body representation
    inline btRigidBody *getInternalRigidBody() const { return m_internal_rigidbody; }

    /**
     * computes the volume of this rigidbody that is currently submerged under water.
     * this computation is done on the GPU and requires a pre-rendered heightmap
     * TODO: the initial render from this function can be used for the silhouette pyramid
     *
     * @param Heightmap to look up the water depth from
     * @param Framebuffer object to render the buoyancy image to. The resolution should be
     *        in accordance with the value in RigidBodyConstants, and the internal format is
     *        assumed to be floating point
     * @param Current width of the screen, used to restore matrix state after computations
     * @param Current height of the screen, used to restore matrix state after computations
     */
    btScalar computeSubmergedVolume(GLuint heightmap, QGLFramebufferObject *framebuffer, QGLShaderProgram *buoyancy_shader, int screen_width, int screen_height, GLfloat *lowres_buffer);

    /* force computations */
    void applyBuoyancy(btScalar buoyancy, const btVector3 &volume_centroid);
    void applyLiftAndDrag();
protected:

    // Poolable methods
    void onAlloc();
    void onFree();

    // function that will be called for rendering.
    void (*m_render_function)();


    /* internal bullet representation for the rigid body simulation. Has to be reinitialized whenever a body is spawned. */
    btRigidBody *m_internal_rigidbody;
    /*
     * collision shape of this object. Should be set by initialize
     * This object is not owned by RigidBody, as collision shapes are shared among instances for performance reasons.
     * As such, it should not be deleted by RigidBody
     */
    btCollisionShape *m_internal_collisionshape;
    /*
     * default motion state, which will be set when initialize is called on this object
     */
    btDefaultMotionState *m_internal_defaultmotionstate;
};

#endif // RIGIDBODY_H
