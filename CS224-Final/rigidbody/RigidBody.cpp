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
    m_internal_rigidbody->applyCentralForce(force); // TODO: use volume_centroid
}

void RigidBody::applyLiftAndDrag(GLuint heightmap, QGLFramebufferObject *framebuffer, QGLShaderProgram *lift_drag_shader, int screen_width, int screen_height, GLfloat *lowres_buffer)
{
    glDisable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_2D);

    framebuffer->bind();

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, BUOYANCY_IMAGE_RESOLUTION, BUOYANCY_IMAGE_RESOLUTION);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // sorry, I'm hard-coding cube code here. Other shapes are overrated
    // TODO: do shit
    const float R = (float)BUOYANCY_IMAGE_RESOLUTION;
    glBegin(GL_POINTS);
    {
        float xpos = -1.0 + 1.0/R, ypos = -1.0 + 1.0/R;
        glColor3f(1.0,0.0,0.0);
        glVertex2f(xpos, ypos);
        glColor3f(1.0,1.0,0.0);
        glVertex2f(xpos += 2.0/R, ypos);
        glColor3f(1.0,0.0,1.0);
        glVertex2f(xpos += 2.0/R, ypos);
        glColor3f(0.0,1.0,1.0);
        glVertex2f(xpos += 2.0/R, ypos);
        glColor3f(0.0,0.0,1.0);
        glVertex2f(xpos += 2.0/R, ypos);
        glColor3f(1.0,1.0,1.0);
        glVertex2f(xpos += 2.0/R, ypos);
    }
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, screen_width, screen_height);

    // read the first 6 pixels of the data back. Each will hold data that corresponds to a face
    glReadPixels(0,0,6,1, GL_RGB, GL_FLOAT, lowres_buffer);

    framebuffer->release();

    for (int i = 0; i < 18; i += 3)
        logln(lowres_buffer[i]);
    logln("--");

    //--- testing:
    {
        glBindTexture(GL_TEXTURE_2D, framebuffer->texture());
        glBegin(GL_QUADS);
        {
            glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, 0.0);
            glTexCoord2f(1.0, 0.0); glVertex2f(1.0, 0.0);
            glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 2.0);
            glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 2.0);
        }
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    //---

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);
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
