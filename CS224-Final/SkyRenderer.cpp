#include "SkyRenderer.h"
#include "Camera.h"
#include "GLFileLoader.h"

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
    QString cube[6];
    cube[0] = ":/posx";
    cube[1] = ":/negx";
    cube[2] = ":/posy";
    cube[3] = ":/negy";
    cube[4] = ":/posz";
    cube[5] = ":/negz";

    if (!GLFileLoader::loadCubeMap(cube, skytexture))
        skytexture = 0;
}

SkyRenderer::~SkyRenderer()
{
    glDeleteTextures(1, &skytexture);
    glDeleteLists(skybox, 1);
}

void SkyRenderer::renderSkyBox(Camera *camera)
{
    // The sky box should appear to be infinitely far away, so don't render it to the depth buffer,
    // so that it doesn't occlude anything in the scene
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    Vector4 center = camera->getLook(), up = camera->getUp();
    // To give the impression of infinity, make the skybox seem to "move with the camera"
    gluLookAt(0.0, 0.0, 0.0,
              center.x, center.y, center.z,
              up.x, up.y, up.z);
   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // TODO: remove this line later
    glBindTexture(GL_TEXTURE_CUBE_MAP, skytexture);
    glCallList(skybox);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}
