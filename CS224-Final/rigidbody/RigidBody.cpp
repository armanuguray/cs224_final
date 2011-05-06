#include "RigidBody.h"
#include "RigidBodyRendering.h"

#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include "RigidBodyConstants.h"
#include <QDebug>

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
    {
        btScalar m[15];
        btTransform t;
        m_internal_rigidbody->getMotionState()->getWorldTransform(t);
        t.getOpenGLMatrix(m);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glMultMatrixf(m);
        m_render_function();
        glPopMatrix();
    }
}

void RigidBody::applyBuoyancy(btScalar buoyancy, const btVector3 &volume_centroid)
{
    // TODO: compute the submerged volume and use it to apply buoyancy
}

void RigidBody::applyLiftAndDrag()
{
    // TODO:
}

btScalar RigidBody::computeSubmergedVolume(GLuint heightmap, QGLFramebufferObject *framebuffer, QGLShaderProgram *buoyancy_shader, int screen_width, int screen_height, GLfloat *lowres_buffer)
{
    // TODO: bind framebuffer
    // TODO: render body using orthogonal projection in object space using a special shader (need a mapping to heightmap coordinates)
    // TODO: loop through the buoyancy image to compute total volume and the buoyancy force, which will be used to apply the buoyancy force

    // enable texture2d
    glDisable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_CULL_FACE);

    framebuffer->bind();
    buoyancy_shader->bind();

    glClear(GL_COLOR_BUFFER_BIT);
    btScalar m[16];
    btTransform t;
    m_internal_rigidbody->getMotionState()->getWorldTransform(t);
    t.getOpenGLMatrix(m);
    glViewport(0,0,BUOYANCY_IMAGE_RESOLUTION,BUOYANCY_IMAGE_RESOLUTION);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    // all objects have unit volume for simplicity
    glOrtho(-1.75,1.75,-1.75,1.75,0,3.5);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    btVector3 &translate = t.getOrigin();
    gluLookAt(translate.x(), translate.y() + 1.75, translate.z(),
              translate.x(), translate.y(), translate.z(),
              0, 0, 1);
    // pass the ctm to the shader (sorry for this loop)
    GLfloat ctm[4][4];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ctm[i][j] = m[4*i + j];
    buoyancy_shader->setUniformValue("ctm", ctm);
    buoyancy_shader->setUniformValue("max_abs_height", 20.0f);
    glMultMatrixf(m);
    m_render_function();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // restore viewport
    glViewport(0,0,screen_width,screen_height);

    buoyancy_shader->release();
    framebuffer->release();

    // enable again, as the rendering function will disable it
    glDisable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    // THIS IS FOR TESTING
    glBindTexture(GL_TEXTURE_2D, framebuffer->texture());
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 1.0, -0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, 1.0, 0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 1.0, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(0.5, 1.0, -0.5);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    // reenable cubemap
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);
    return 0; // TODO: return real value
}
