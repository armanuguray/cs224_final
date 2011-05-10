#include "RigidBody.h"
#include "RigidBodyRendering.h"

#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include "RigidBodyConstants.h"
#include <QDebug>
#include "ProjectorCamera.h"
#include "WaveParticleManager.h"
#include "WaveConstants.h"

#define TV_MODE

RigidBody::RigidBody(ProjectorCamera *camera, WaveParticleManager *waveparticlemanager)
{
    m_camera = camera;
    m_wpmanager = waveparticlemanager;

    m_internal_rigidbody = NULL;
    m_internal_collisionshape = NULL;
    m_internal_defaultmotionstate = NULL;
    m_render_function = NULL;

    const float extent = SIDE_LENGTH / 2.0f;
    centroids[0] = btVector3(-extent, -extent/3.0, extent/3.0);
    centroids[1] = btVector3(-extent, extent/3.0, -extent/3.0);
    centroids[2] = btVector3(extent, -extent/3.0, extent/3.0);
    centroids[3] = btVector3(extent, extent/3.0, -extent/3.0);
    centroids[4] = btVector3(-extent/3.0, -extent, extent/3.0);
    centroids[5] = btVector3(extent/3.0, -extent, -extent/3.0);
    centroids[6] = btVector3(-extent/3.0, extent, extent/3.0);
    centroids[7] = btVector3(extent/3.0, extent, -extent/3.0);
    centroids[8] = btVector3(-extent/3.0, extent/3.0, -extent);
    centroids[9] = btVector3(extent/3.0, -extent/3.0, -extent);
    centroids[10] = btVector3(-extent/3.0, extent/3.0, extent);
    centroids[11] = btVector3(extent/3.0, -extent/3.0, extent);
//    centroids[0] = btVector3(0.0, 0.0, 0.0);
//    centroids[1] = btVector3(0.0, 0.0, 0.0);
//    centroids[2] = btVector3(0.0,  0.0, 0.0);
//    centroids[3] = btVector3(0.0,  0.0, 0.0);
//    centroids[4] = btVector3(0.0, 0.0, 0.0);
//    centroids[5] = btVector3(0.0, 0.0, 0.0);
//    centroids[6] = btVector3(0.0, 0.0, 0.0);
//    centroids[7] = btVector3(0.0, 0.0, 0.0);
//    centroids[8] = btVector3(0.0, 0.0, 0.0);
//    centroids[9] = btVector3(0.0, 0.0, 0.0);
//    centroids[10] = btVector3(0.0, 0.0, 0.0);
//    centroids[11] = btVector3(0.0, 0.0, 0.0);
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
    m_internal_rigidbody->setActivationState(DISABLE_DEACTIVATION);
}

void RigidBody::render()
{
    if (m_render_function != NULL)
    {
        static btScalar m[16];
        static btTransform t;
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

void RigidBody::applyLiftAndDrag(GLuint heightmap, QGLFramebufferObject *framebuffer, QGLShaderProgram *liftdrag_shader, int screen_width, int screen_height, GLfloat *lowres_buffer)
{
    glDisable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_2D);

//#define DIARRHEA
#ifdef DIARRHEA
     glBindTexture(GL_TEXTURE_2D, m_wpmanager->velocityTexture());
     glBegin(GL_QUADS);
     glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, 10.0, -100.0);
     glTexCoord2f(1.0, 0.0); glVertex3f(-100.0, 10.0, 100.0);
     glTexCoord2f(1.0, 1.0); glVertex3f(100.0, 10.0, 100.0);
     glTexCoord2f(0.0, 1.0); glVertex3f(100.0, 10.0, -100.0);
     glEnd();
#endif

    // set uniforms
    framebuffer->bind();
    liftdrag_shader->bind();
    btScalar m[16];
    btTransform t;
    m_internal_rigidbody->getMotionState()->getWorldTransform(t);
    t.getOpenGLMatrix(m);
    GLfloat ctm[4][4];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ctm[i][j] = m[4*i + j];

    int w = WAVE_HEIGHTMAP_WIDTH,
        h = WAVE_HEIGHTMAP_HEIGHT;

    float originX = m_wpmanager->heightmapOriginX(),
          originZ = m_wpmanager->heightmapOriginZ();

    liftdrag_shader->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    liftdrag_shader->setUniformValue("wave_speed", (GLfloat)WAVE_SPEED);
    liftdrag_shader->setUniformValue("htl", QVector3D(-w * .5 + originX, 0, -h * .5 + originZ));
    liftdrag_shader->setUniformValue("htr", QVector3D( w * .5 + originX, 0, -h * .5 + originZ));
    liftdrag_shader->setUniformValue("hbl", QVector3D(-w * .5 + originX, 0,  h * .5 + originZ));
    liftdrag_shader->setUniformValue("hbr", QVector3D( w * .5 + originX, 0,  h * .5 + originZ));
    liftdrag_shader->setUniformValue("tl", QVector3D(m_camera->ul.x / m_camera->ul.w, m_camera->ul.y / m_camera->ul.w, m_camera->ul.z / m_camera->ul.w));
    liftdrag_shader->setUniformValue("tr", QVector3D(m_camera->ur.x / m_camera->ur.w, m_camera->ur.y / m_camera->ur.w, m_camera->ur.z / m_camera->ur.w));
    liftdrag_shader->setUniformValue("bl", QVector3D(m_camera->ll.x / m_camera->ll.w, m_camera->ll.y / m_camera->ll.w, m_camera->ll.z / m_camera->ll.w));
    liftdrag_shader->setUniformValue("br", QVector3D(m_camera->lr.x / m_camera->lr.w, m_camera->lr.y / m_camera->lr.w, m_camera->lr.z / m_camera->lr.w));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_wpmanager->heightTexture());
    liftdrag_shader->setUniformValue("heightmap", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_wpmanager->velocityTexture());
    liftdrag_shader->setUniformValue("velocitymap", 1);

    liftdrag_shader->setUniformValue("ctm", ctm);
    liftdrag_shader->setUniformValue("Cd", DRAG_COEFFICIENT);
    liftdrag_shader->setUniformValue("Cl", LIFT_COEFFICIENT);
    liftdrag_shader->setUniformValue("ro", WATER_DENSITY);
    liftdrag_shader->setUniformValue("Atotal", SIDE_LENGTH*SIDE_LENGTH/2.0f);
    liftdrag_shader->setUniformValue("epsilon", LD_EPSILON);

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, BUOYANCY_IMAGE_RESOLUTION, BUOYANCY_IMAGE_RESOLUTION);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Send triangle faces to the gpu. (sorry, I'm hard-coding cube code here. Other shapes are overrated)
    const float extent = SIDE_LENGTH / 2.0f;
    const float R = (float)BUOYANCY_IMAGE_RESOLUTION;
    float xpos = -1.0 + 1.0/R, ypos = -1.0 + 1.0/R;
    glBegin(GL_POINTS);
    {
        // -x
        // 1.
        glNormal3f(-1.0, 0.0, 0.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, -extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1, -extent, -extent,  extent);
        glMultiTexCoord3f(GL_TEXTURE2, -extent,  extent,  extent);
        btVector3 velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[0]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos, ypos); // pixel
        // 2.
        glNormal3f(-1.0, 0.0, 0.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, -extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1, -extent,  extent,  extent);
        glMultiTexCoord3f(GL_TEXTURE2, -extent,  extent, -extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[1]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel

        // +x
        // 1.
        glNormal3f(1.0, 0.0, 0.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, extent, -extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1, extent, -extent,  extent);
        glMultiTexCoord3f(GL_TEXTURE2, extent,  extent,  extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[2]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel
        // 2.
        glNormal3f(1.0, 0.0, 0.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, extent, -extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1, extent,  extent,  extent);
        glMultiTexCoord3f(GL_TEXTURE2, extent,  extent, -extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[3]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel

        // -y
        // 1.
        glNormal3f(0.0, -1.0, 0.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, -extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1, -extent, -extent,  extent);
        glMultiTexCoord3f(GL_TEXTURE2,  extent, -extent,  extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[4]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel
        // 2.
        glNormal3f(0.0, -1.0, 0.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, -extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1,  extent, -extent,  extent);
        glMultiTexCoord3f(GL_TEXTURE2,  extent, -extent, -extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[5]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel

        // +y
        // 1.
        glNormal3f(0.0, 1.0, 0.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1, -extent, extent,  extent);
        glMultiTexCoord3f(GL_TEXTURE2,  extent, extent,  extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[6]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel
        // 2.
        glNormal3f(0.0, 1.0, 0.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1,  extent, extent,  extent);
        glMultiTexCoord3f(GL_TEXTURE2,  extent, extent, -extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[7]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel

        // -z
        // 1.
        glNormal3f(0.0, 0.0, -1.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, -extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1, -extent,  extent, -extent);
        glMultiTexCoord3f(GL_TEXTURE2,  extent,  extent, -extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[8]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel
        // 2.
        glNormal3f(0.0, 0.0, -1.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, -extent, -extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1,  extent,  extent, -extent);
        glMultiTexCoord3f(GL_TEXTURE2,  extent, -extent, -extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[9]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel

        // +z
        // 1.
        glNormal3f(0.0, 0.0, 1.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, -extent, extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1, -extent,  extent, extent);
        glMultiTexCoord3f(GL_TEXTURE2,  extent,  extent, extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[10]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel
        // 2.
        glNormal3f(0.0, 0.0, 1.0); // normal
        glMultiTexCoord3f(GL_TEXTURE0, -extent, -extent, extent); // corners
        glMultiTexCoord3f(GL_TEXTURE1,  extent,  extent, extent);
        glMultiTexCoord3f(GL_TEXTURE2,  extent, -extent, extent);
        velocity = m_internal_rigidbody->getVelocityInLocalPoint(centroids[11]); // face velocity
        glMultiTexCoord3f(GL_TEXTURE3, velocity.x(), velocity.y(), velocity.z());
        glVertex2f(xpos += 2.0/R, ypos); // pixel
    }
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, screen_width, screen_height);

    // read the first 12 pixels of the data back. Each will hold data that corresponds to a triangle
    glReadPixels(0,0,12,1, GL_RGB, GL_FLOAT, lowres_buffer);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    liftdrag_shader->release();
    framebuffer->release();
    btVector3 force;
    int cindex = 0;
    for (int i = 0; i < 36; i += 3) {
        force = btVector3(lowres_buffer[i], lowres_buffer[i+1], lowres_buffer[i+2]);
        if (force.length2() > 0.0)
            m_internal_rigidbody->applyForce(force, centroids[cindex]);
        cindex++;
    }
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

    int w = WAVE_HEIGHTMAP_WIDTH,
        h = WAVE_HEIGHTMAP_HEIGHT;

    float originX = m_wpmanager->heightmapOriginX(),
          originZ = m_wpmanager->heightmapOriginZ();

    buoyancy_shader->setUniformValue("wp_max_amplitude", (GLfloat)WAVE_MAX_AMPLITUDE);
    buoyancy_shader->setUniformValue("htl", QVector3D(-w * .5 + originX, 0, -h * .5 + originZ));
    buoyancy_shader->setUniformValue("htr", QVector3D( w * .5 + originX, 0, -h * .5 + originZ));
    buoyancy_shader->setUniformValue("hbl", QVector3D(-w * .5 + originX, 0,  h * .5 + originZ));
    buoyancy_shader->setUniformValue("hbr", QVector3D( w * .5 + originX, 0,  h * .5 + originZ));
    buoyancy_shader->setUniformValue("tl", QVector3D(m_camera->ul.x / m_camera->ul.w, m_camera->ul.y / m_camera->ul.w, m_camera->ul.z / m_camera->ul.w));
    buoyancy_shader->setUniformValue("tr", QVector3D(m_camera->ur.x / m_camera->ur.w, m_camera->ur.y / m_camera->ur.w, m_camera->ur.z / m_camera->ur.w));
    buoyancy_shader->setUniformValue("bl", QVector3D(m_camera->ll.x / m_camera->ll.w, m_camera->ll.y / m_camera->ll.w, m_camera->ll.z / m_camera->ll.w));
    buoyancy_shader->setUniformValue("br", QVector3D(m_camera->lr.x / m_camera->lr.w, m_camera->lr.y / m_camera->lr.w, m_camera->lr.z / m_camera->lr.w));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_wpmanager->heightTexture());
    buoyancy_shader->setUniformValue("heightmap", 0);

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

    glBindTexture(GL_TEXTURE_2D, 0);

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
                              GLfloat *lowres_buffer, int screen_width, int screen_height, float now)
{
    static int ct = 0;
    ct++;

    QGLShaderProgram *wavegen_shader = shaders["wavegen"];
    QGLFramebufferObject *lowres_fb = buffers["low-res"];
    QGLShaderProgram *waveeffect_shader = shaders["waveeffect"];
    QGLFramebufferObject *lowres_fb2 = buffers["low-res2"];
    QGLShaderProgram *dir_shader = shaders["computedir"];
    QGLFramebufferObject *dir_gather = buffers["low-res3"];

    QGLShaderProgram *downscale = shaders["downscale"];
    QGLShaderProgram *upscale = shaders["upscale"];
    QMap<GLint, QGLFramebufferObject *> downscale_map;
    downscale_map[16] = dir_gather;
    downscale_map[8] = buffers["8x8"];
    downscale_map[4] = buffers["4x4"];
    downscale_map[2] = buffers["2x2"];
    downscale_map[1] = buffers["1x1"];

    QMap<GLint, QGLFramebufferObject *> upscale_map;
    upscale_map[16] = lowres_fb2;
    upscale_map[8] = buffers["8x8 2"];
    upscale_map[4] = buffers["4x4 2"];
    upscale_map[2] = buffers["2x2 2"];
    upscale_map[1] = buffers["1x1"];

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
            glNormal3f(1.0, 0.0, 0.0); glVertex3f(half_extent, 0, 0); buffers["1x1 2"];

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

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_CUBE_MAP);

        waveeffect_shader->release();
        lowres_fb2->release();
    }

    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        gluLookAt(0, 1, 0, 0, 0, 0, 0, 0, 1);

        glDisable(GL_TEXTURE_CUBE_MAP);
        glEnable(GL_TEXTURE_2D);

        // pass 3: find boundary pixels and directions
        {
            dir_gather->bind();
            dir_shader->bind();

            glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, lowres_fb->texture());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, lowres_fb2->texture());

            dir_shader->setUniformValue("silhouette", 0);
            dir_shader->setUniformValue("effects", 1);

            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(-halfextent, 0.0, -halfextent);
            glTexCoord2f(1.0, 0.0); glVertex3f(-halfextent, 0.0, halfextent);
            glTexCoord2f(1.0, 1.0); glVertex3f(halfextent, 0.0, halfextent);
            glTexCoord2f(0.0, 1.0); glVertex3f(halfextent, 0.0, -halfextent);
            glEnd();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);

            dir_shader->release();
            dir_gather->release();
        }

        // pass 4: downscale
        {
            GLint old_size = BUOYANCY_IMAGE_RESOLUTION;

            downscale->bind();

            for (GLint new_size = old_size / 2; new_size > 0; new_size /= 2)
            {
                glViewport(0, 0, new_size, new_size);
                downscale_map[new_size]->bind();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, downscale_map[2 * new_size]->texture());

                downscale->setUniformValue("prev_texture", 0);
                downscale->setUniformValue("new_size", (GLfloat) new_size);

                glClear(GL_COLOR_BUFFER_BIT);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0, 0.0); glVertex3f(-halfextent, 0.0, -halfextent);
                glTexCoord2f(1.0, 0.0); glVertex3f(-halfextent, 0.0, halfextent);
                glTexCoord2f(1.0, 1.0); glVertex3f(halfextent, 0.0, halfextent);
                glTexCoord2f(0.0, 1.0); glVertex3f(halfextent, 0.0, -halfextent);
                glEnd();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);

                downscale_map[new_size]->release();
            }

            downscale->release();

            glViewport(0, 0, old_size, old_size);
        }

        // pass 5: upscale
        {
            GLint max_size = BUOYANCY_IMAGE_RESOLUTION;

            upscale->bind();

            for (GLint new_size = 2; new_size <= max_size; new_size *= 2)
            {
                glViewport(0, 0, new_size, new_size);
                upscale_map[new_size]->bind();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, upscale_map[new_size / 2]->texture());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, downscale_map[new_size]->texture());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, downscale_map[new_size / 2]->texture());

                upscale->setUniformValue("prev_texture", 0);
                upscale->setUniformValue("downscale_texture", 1);
                upscale->setUniformValue("downscale_prev_texture", 2);
                upscale->setUniformValue("new_size", (GLfloat) new_size);

                glClear(GL_COLOR_BUFFER_BIT);
                glBegin(GL_QUADS);
                glTexCoord2f(0.0, 0.0); glVertex3f(-halfextent, 0.0, -halfextent);
                glTexCoord2f(1.0, 0.0); glVertex3f(-halfextent, 0.0, halfextent);
                glTexCoord2f(1.0, 1.0); glVertex3f(halfextent, 0.0, halfextent);
                glTexCoord2f(0.0, 1.0); glVertex3f(halfextent, 0.0, -halfextent);
                glEnd();

                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);

                upscale_map[new_size]->release();
            }

            upscale->release();

            glViewport(0, 0, max_size, max_size);
        }

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_CUBE_MAP);

        glPopMatrix();
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // generate the wave particles
   {
        static const int buffer_size = 4 * BUOYANCY_IMAGE_RESOLUTION * BUOYANCY_IMAGE_RESOLUTION;
        static GLfloat buffer[buffer_size];

        lowres_fb2->bind();
        glReadPixels(0, 0, BUOYANCY_IMAGE_RESOLUTION, BUOYANCY_IMAGE_RESOLUTION, GL_RGBA, GL_FLOAT, buffer);
        lowres_fb2->release();

        float left = center.x() - halfextent;
        float bottom = center.z() + halfextent;
        float unit = OBJ_EXTENT / BUOYANCY_IMAGE_RESOLUTION;

        for (int idx = 0, j = 0; idx < buffer_size; idx += 4 * 2, j++) {
            GLfloat &direct = buffer[idx];
            GLfloat &indirect = buffer[idx + 1];
            GLfloat &dir_x = buffer[idx + 2];
            GLfloat &dir_y = buffer[idx + 3];

            // TODO refactor this into a header somewhere
            static const float EPS = 0.00001;

            // This is the BS constant
            static float amp_scale_constant = .008f;

            int x = j % BUOYANCY_IMAGE_RESOLUTION;
            int y = j / BUOYANCY_IMAGE_RESOLUTION;

            Vector2 loc(left + x * unit, bottom - y * unit);
            Vector2 vel = Vector2(dir_x, dir_y);
            int mag = vel.getMagnitude();
            if (mag > EPS) {
                vel /= mag;
            } else {
                continue;
            }

            // generate indirect effect particles
            if (fabs(indirect) > EPS) {
                // HIS PAPER SAYS TO COMPUTE THIS VALUE WTF
                //static const float rad = WAVE_PARTICLE_RADIUS / 10;
                float amp = indirect / (M_PI / 2.f * (unit / 2) * (unit / 2)) * amp_scale_constant;
                amp = max(0.00001f, min(amp, 0.06f));
//                // compute the dispersion angle, yo
//                float dispersionAngle = 0.f;
//                int ct = 0;

//                int right = idx + 4;
//                if (x < BUOYANCY_IMAGE_RESOLUTION - 1 && fabs(buffer[right + 1]) > EPS) {
//                    float d = Vector2(buffer[right + 2], buffer[right + 3]).getNormalized().dot(vel);

//                    dispersionAngle += acos(d);
//                    ct++;
//                }

//                int left = idx - 4;
//                if (x >= 1 && fabs(buffer[left + 1]) > EPS) {
//                    float d = Vector2(buffer[left + 2], buffer[left + 3]).getNormalized().dot(vel);

//                    dispersionAngle += acos(d);
//                    ct++;
//                }

//              int top = idx + 4 * BUOYANCY_IMAGE_RESOLUTION;
//                if (y < BUOYANCY_IMAGE_RESOLUTION - 1 && fabs(buffer[top + 1]) > EPS) {
//                    float d = Vector2(buffer[top + 2], buffer[top + 3]).getNormalized().dot(vel);
//                    dispersionAngle += acos(d);
//                    ct++;
//                }

//                int bottom = idx - 4 * BUOYANCY_IMAGE_RESOLUTION;
//                if (y >= 1 && fabs(buffer[bottom + 1]) > EPS) {
//                    float d = Vector2(buffer[bottom + 2], buffer[bottom + 3]).getNormalized().dot(vel);
//                    dispersionAngle += acos(d);
//                    ct++;
//                }

                if ((rand() % 100) < 60) {
             //   logln(amp);

//                                    logln("here " << amp);
                                    manager.generateWaveParticle(loc + vel * 1.5, vel, 1.2, amp, now);
                //manager.generateUniformWave(3, loc, amp, now);
            }
           }

            if (fabs(direct) > EPS) {
//                float amp = direct / (M_PI / 2.f * WAVE_PARTICLE_RADIUS * WAVE_PARTICLE_RADIUS) * amp_scale_constant / 5;
                float amp = indirect / (M_PI / 2.f * (unit / 2) * (unit / 2)) * amp_scale_constant * 10;
                amp = max(0.00001f, min(amp, 0.06f));

//                logln("here " << amp);
//                manager.generateUniformWave(3, loc + vel * 1.5, amp, now);
                if ((rand() % 100) < 60) {
             //   logln(amp);

                                    manager.generateWaveParticle(loc + vel * 2, vel, 1.2, amp, now);
                //manager.generateUniformWave(3, loc, amp, now);
            }
             }
        }
    }

    // restore the viewport
    glViewport(0, 0, screen_width, screen_height);

#ifdef TV_MODE
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

        startx += w + s;
        glBindTexture(GL_TEXTURE_2D, dir_gather->texture());
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
        glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
        glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
        glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
        glEnd();

        {
            startx += w + s;
            glBindTexture(GL_TEXTURE_2D, downscale_map[8]->texture());
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
            glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
            glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
            glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
            glEnd();

            startx += w + s;
            glBindTexture(GL_TEXTURE_2D, downscale_map[4]->texture());
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
            glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
            glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
            glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
            glEnd();

            startx += w + s;
            glBindTexture(GL_TEXTURE_2D, downscale_map[2]->texture());
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
            glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
            glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
            glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
            glEnd();

            startx += w + s;
            glBindTexture(GL_TEXTURE_2D, downscale_map[1]->texture());
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
            glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
            glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
            glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
            glEnd();
        }

        {
            startx += w + s;
            glBindTexture(GL_TEXTURE_2D, upscale_map[1]->texture());
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
            glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
            glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
            glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
            glEnd();

            startx += w + s;
            glBindTexture(GL_TEXTURE_2D, upscale_map[2]->texture());
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
            glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
            glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
            glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
            glEnd();

            startx += w + s;
            glBindTexture(GL_TEXTURE_2D, upscale_map[4]->texture());
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
            glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
            glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
            glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
            glEnd();

            startx += w + s;
            glBindTexture(GL_TEXTURE_2D, upscale_map[8]->texture());
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(startx, 1.0, startz);
            glTexCoord2f(1.0, 0.0); glVertex3f(startx, 1.0, startz + w);
            glTexCoord2f(1.0, 1.0); glVertex3f(startx + w, 1.0, startz + w);
            glTexCoord2f(0.0, 1.0); glVertex3f(startx + w, 1.0, startz);
            glEnd();
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_CUBE_MAP);
    }
#endif
}
