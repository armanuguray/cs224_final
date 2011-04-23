#include "SkyRenderer.h"
#include "Camera.h"

SkyRenderer::SkyRenderer()
{
    // compile skybox rendering code and upload it to the gpu
    skybox = glGenLists(1);
    glNewList(skybox, GL_COMPILE);
    {
        glColor3f(1.0,1.0,1.0);
        glBegin(GL_QUADS);
        {
            glTexCoord3f(1.0f,-1.0f,1.0f); glVertex3f(1.0,-1.0,1.0);
            glTexCoord3f(-1.0f,-1.0f,1.0f);glVertex3f(-1.0,-1.0,1.0);
            glTexCoord3f(-1.0f,1.0f,1.0f);glVertex3f(-1.0,1.0,1.0);
            glTexCoord3f(1.0f,1.0f,1.0f); glVertex3f(1.0,1.0,1.0);

            glTexCoord3f(-1.0f,-1.0f,1.0f);glVertex3f(-1.0,-1.0,1.0);
            glTexCoord3f(-1.0f,-1.0f,-1.0f); glVertex3f(-1.0,-1.0,-1.0);
            glTexCoord3f(-1.0f,1.0f,-1.0f);  glVertex3f(-1.0,1.0,-1.0);
            glTexCoord3f(-1.0f,1.0f,1.0f); glVertex3f(-1.0,1.0,1.0);

            glTexCoord3f(-1.0f,-1.0f,-1.0f);  glVertex3f(-1.0,-1.0,-1.0);
            glTexCoord3f(1.0f,-1.0f,-1.0f); glVertex3f(1.0,-1.0,-1.0);
            glTexCoord3f(1.0f,1.0f,-1.0f);  glVertex3f(1.0,1.0,-1.0);
            glTexCoord3f(-1.0f,1.0f,-1.0f); glVertex3f(-1.0,1.0,-1.0);

            glTexCoord3f(1.0f,-1.0f,-1.0f); glVertex3f(1.0,-1.0,-1.0);
            glTexCoord3f(1.0f,-1.0f,1.0f);glVertex3f(1.0,-1.0,1.0);
            glTexCoord3f(1.0f,1.0f,1.0f); glVertex3f(1.0,1.0,1.0);
            glTexCoord3f(1.0f,1.0f,-1.0f);glVertex3f(1.0,1.0,-1.0);

            glTexCoord3f(-1.0f,-1.0f,-1.0f);glVertex3f(-1.0,-1.0,-1.0);
            glTexCoord3f(-1.0f,-1.0f,1.0f);glVertex3f(-1.0,-1.0,1.0);
            glTexCoord3f(1.0f,-1.0f,1.0f);glVertex3f(1.0,-1.0,1.0);
            glTexCoord3f(1.0f,-1.0f,-1.0f);glVertex3f(1.0,-1.0,-1.0);

            glTexCoord3f(-1.0f,1.0f,-1.0f);glVertex3f(-1.0,1.0,-1.0);
            glTexCoord3f(1.0f,1.0f,-1.0f);glVertex3f(1.0,1.0,-1.0);
            glTexCoord3f(1.0f,1.0f,1.0f); glVertex3f(1.0,1.0,1.0);
            glTexCoord3f(-1.0f,1.0f,1.0f);glVertex3f(-1.0,1.0,1.0);
        }
        glEnd();
    }
    glEndList();

    // load skybox texture

}

void SkyRenderer::renderSkyBox(Camera *camera)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    Vector4 center = camera->getLook(), up = camera->getUp();
    gluLookAt(0.0, 0.0, 0.0,
              center.x, center.y, center.z,
              up.x, up.y, up.z);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glCallList(skybox);
    glPopMatrix();
}
