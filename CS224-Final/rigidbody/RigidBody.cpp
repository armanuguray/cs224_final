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

void RigidBody::applyBuoyancy(btScalar submerged_volume, const btVector3 &volume_centroid)
{
    btVector3 force = btVector3(0.0, GRAVITY, 0.0) * WATER_DENSITY * submerged_volume;
//    btVector3 force = btVector3(0.0, GRAVITY, 0.0) * WATER_DENSITY * submerged_volume * .8;
    m_internal_rigidbody->applyCentralForce(force); // TODO: use volume_centroid
}

void RigidBody::applyLiftAndDrag()
{
    // TODO:
}

btScalar RigidBody::computeSubmergedVolume(GLuint heightmap, QGLFramebufferObject *framebuffer, QGLShaderProgram *buoyancy_shader, int screen_width, int screen_height, GLfloat *lowres_buffer, btVector3 &out_centroid)
{
    // TODO: output volume centroid

    // enable texture2d
    glDisable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_CULL_FACE);

    // bind offscreen framebuffer and shader
    framebuffer->bind();
    buoyancy_shader->bind();

    // render the object as seen from a top orthogonal view
    glClear(GL_COLOR_BUFFER_BIT);
    btScalar m[16];
    btTransform t;
    m_internal_rigidbody->getMotionState()->getWorldTransform(t);
    t.getOpenGLMatrix(m);
    glViewport(0,0,BUOYANCY_IMAGE_RESOLUTION,BUOYANCY_IMAGE_RESOLUTION);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    // all objects have 2x2x2 volume for simplicity
    const float halfextent = OBJ_EXTENT/2.0f;
    glOrtho(-halfextent,halfextent,-halfextent,halfextent,0,OBJ_EXTENT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    btVector3 &translate = t.getOrigin();
    gluLookAt(translate.x(), translate.y() + halfextent, translate.z(),
              translate.x(), translate.y(), translate.z(),
              0, 0, 1);
    // pass the ctm to the shader (sorry for this loop)
    GLfloat ctm[4][4];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ctm[i][j] = m[4*i + j];
    buoyancy_shader->setUniformValue("ctm", ctm);
    glMultMatrixf(m);
    m_render_function();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // restore viewport
    glViewport(0,0,screen_width,screen_height);

    // transfer the texture to the CPU
    glReadPixels(0,0,BUOYANCY_IMAGE_RESOLUTION,BUOYANCY_IMAGE_RESOLUTION,GL_RGB,GL_FLOAT,lowres_buffer);

    buoyancy_shader->release();
    framebuffer->release();

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    /*
     * (these get disabled by m_render_function())
     * glDisable(GL_TEXTURE_2D);
     * glEnable(GL_TEXTURE_CUBE_MAP);
     */

    // compute volume
    btScalar volume = 0;
    for (unsigned i = 0; i < BUOYANCY_IMAGE_RESOLUTION*BUOYANCY_IMAGE_RESOLUTION*3; i += 3)
        volume += lowres_buffer[i];
    btScalar unit_area = OBJ_EXTENT/(btScalar)BUOYANCY_IMAGE_RESOLUTION; unit_area *= unit_area;
    volume *= unit_area;
    return volume;
}

void RigidBody::generateWaves(WaveParticleManager &manager,
                              QMap<QString, QGLShaderProgram *> &shaders,
                              QMap<QString, QGLFramebufferObject *> &buffers,
                              GLfloat *lowres_buffer, int screen_width, int screen_height)
{
    QGLShaderProgram *wavegen_shader = shaders["wavegen"];
    QGLShaderProgram *waveeffect_shader = shaders["waveeffect"];
    QGLFramebufferObject *lowres_fb = buffers["low-res"];
    QGLFramebufferObject *lowres_fb2 = buffers["low-res2"];

    static const float halfextent = OBJ_EXTENT / 2.0f;

    static btScalar matrix[16];
    static GLfloat ctm[4][4];
    btTransform transform;
    m_internal_defaultmotionstate->getWorldTransform(transform);
    transform.getOpenGLMatrix(matrix);

    // TODO get rid of this madness
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ctm[i][j] = matrix[4*i + j];

    btVector3 &center = transform.getOrigin();

    glViewport(0, 0, BUOYANCY_IMAGE_RESOLUTION, BUOYANCY_IMAGE_RESOLUTION);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(-halfextent, halfextent, -halfextent, halfextent, 0, OBJ_EXTENT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    gluLookAt(center.x(), center.y() + halfextent, center.z(),
              center.x(), center.y(), center.z(),
              0, 0, 1);
    glMultMatrixf(matrix);

    // this is the initial depth pass
    {
        lowres_fb->bind();
        wavegen_shader->bind();

        wavegen_shader->setUniformValue("world_transform", ctm);
        glClear(GL_COLOR_BUFFER_BIT);
        m_render_function();

        wavegen_shader->release();
        lowres_fb->release();
    }

    // this is the wave effect pass WHAT THE SHIT DOES THIS DO
    {
        const static int vel_idx = 14;
        glBindAttribLocation(waveeffect_shader->programId(), vel_idx, "velocity");

        lowres_fb2->bind();
        waveeffect_shader->bind();
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_TEXTURE_CUBE_MAP);
        glEnable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lowres_fb->texture());
        waveeffect_shader->setUniformValue("overview", 0);
        glActiveTexture(GL_TEXTURE1);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        waveeffect_shader->setUniformValue("world_transform", ctm);
        {
            btVector3 velocity;
            static float half_extent = SIDE_LENGTH / 2.0f;
            glBegin(GL_POINTS);

            velocity = //m_internal_rigidbody->getWorldTransform().getBasis().transpose() *
                       m_internal_rigidbody->getVelocityInLocalPoint(btVector3(0, 0, half_extent));
            glVertexAttrib3f(vel_idx, velocity.getX(), velocity.getY(), velocity.getZ());
            glNormal3f(0.0, 0.0, 1.0); glVertex3f(0, 0, half_extent);

            velocity = //m_internal_rigidbody->getWorldTransform().getBasis().transpose() *
                       m_internal_rigidbody->getVelocityInLocalPoint(btVector3(-half_extent, 0, 0));
            glVertexAttrib3f(vel_idx, velocity.getX(), velocity.getY(), velocity.getZ());
            glNormal3f(-1.0, 0.0, 0.0); glVertex3f(-half_extent, 0, 0);

            velocity = //m_internal_rigidbody->getWorldTransform().getBasis().transpose() *
                       m_internal_rigidbody->getVelocityInLocalPoint(btVector3(0, 0, -half_extent));
            glVertexAttrib3f(vel_idx, velocity.getX(), velocity.getY(), velocity.getZ());
            glNormal3f(0.0, 0.0, -1.0); glVertex3f(0, 0, -half_extent);

            velocity = //m_internal_rigidbody->getWorldTransform().getBasis().transpose() *
                       m_internal_rigidbody->getVelocityInLocalPoint(btVector3(half_extent, 0, 0));
            glVertexAttrib3f(vel_idx, velocity.getX(), velocity.getY(), velocity.getZ());
            glNormal3f(1.0, 0.0, 0.0); glVertex3f(half_extent, 0, 0);

            velocity = //m_internal_rigidbody->getWorldTransform().getBasis().transpose() *
                       m_internal_rigidbody->getVelocityInLocalPoint(btVector3(0, -half_extent, 0));
            glVertexAttrib3f(vel_idx, velocity.getX(), velocity.getY(), velocity.getZ());
            glNormal3f(0.0, -1.0, 0.0); glVertex3f(0, -half_extent, 0);

            velocity = //m_internal_rigidbody->getWorldTransform().getBasis().transpose() *
                       m_internal_rigidbody->getVelocityInLocalPoint(btVector3(0, half_extent, 0));
            glVertexAttrib3f(vel_idx, velocity.getX(), velocity.getY(), velocity.getZ());
            glNormal3f(0.0, 1.0, 0.0); glVertex3f(0, half_extent, 0);

            glEnd();
        }

        glDisable(GL_BLEND);

        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_TEXTURE_CUBE_MAP);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_CUBE_MAP);

        waveeffect_shader->release();
        lowres_fb2->release();
    }

    // pass 3: find boundary pixels and directions
    {

    }

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // restore the viewport
    glViewport(0, 0, screen_width, screen_height);

    {
        // THIS IS FOR TESTING
        const static float w = 2.0f;
        const static float s = 1.0f;

        glDisable(GL_TEXTURE_CUBE_MAP);
        glEnable(GL_TEXTURE_2D);

        float startx = 5.0;
        float startz = 5.0;
        glBindTexture(GL_TEXTURE_2D, lowres_fb->texture());
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
        glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
        glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
        glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
        glEnd();

        startx += w + s;
        glBindTexture(GL_TEXTURE_2D, lowres_fb2->texture());
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
        glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
        glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
        glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_CUBE_MAP);
    }
}
