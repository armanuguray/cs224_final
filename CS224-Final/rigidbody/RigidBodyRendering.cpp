#include "RigidBodyRendering.h"
#include "GLFileLoader.h"

GLuint boxTexture = 0;
GLuint boxList = 0;

void RigidBodyRendering::initialize()
{
    // load textures
    QString s(":/crate");
    GLFileLoader::loadTexture2D(s, boxTexture);

    // load lists
    boxList = glGenLists(1);
    glNewList(boxList, GL_COMPILE);
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        {
            glTexCoord2f(1.0f,1.0f); glVertex3f(1.0,1.0,1.0);
            glTexCoord2f(0.0f,1.0f); glVertex3f(-1.0,1.0,1.0);
            glTexCoord2f(0.0f,0.0f); glVertex3f(-1.0,-1.0,1.0);
            glTexCoord2f(1.0f,0.0f); glVertex3f(1.0,-1.0,1.0);

            glTexCoord2f(1.0f,1.0f); glVertex3f(-1.0,1.0,1.0);
            glTexCoord2f(0.0f,1.0f); glVertex3f(-1.0,1.0,-1.0);
            glTexCoord2f(0.0f,0.0f); glVertex3f(-1.0,-1.0,-1.0);
            glTexCoord2f(1.0f,0.0f); glVertex3f(-1.0,-1.0,1.0);

            glTexCoord2f(1.0f,1.0f); glVertex3f(-1.0,1.0,-1.0);
            glTexCoord2f(0.0f,1.0f); glVertex3f(1.0,1.0,-1.0);
            glTexCoord2f(0.0f,0.0f); glVertex3f(1.0,-1.0,-1.0);
            glTexCoord2f(1.0f,0.0f); glVertex3f(-1.0,-1.0,-1.0);

            glTexCoord2f(1.0f,1.0f); glVertex3f(1.0,1.0,-1.0);
            glTexCoord2f(0.0f,1.0f); glVertex3f(1.0,1.0,1.0);
            glTexCoord2f(0.0f,0.0f); glVertex3f(1.0,-1.0,1.0);
            glTexCoord2f(1.0f,0.0f); glVertex3f(1.0,-1.0,-1.0);

            glTexCoord2f(1.0f,1.0f); glVertex3f(1.0,-1.0,-1.0);
            glTexCoord2f(0.0f,1.0f); glVertex3f(1.0,-1.0,1.0);
            glTexCoord2f(0.0f,0.0f); glVertex3f(-1.0,-1.0,1.0);
            glTexCoord2f(1.0f,0.0f); glVertex3f(-1.0,-1.0,-1.0);

            glTexCoord2f(1.0f,1.0f); glVertex3f(-1.0,1.0,1.0);
            glTexCoord2f(0.0f,1.0f); glVertex3f(1.0,1.0,1.0);
            glTexCoord2f(0.0f,0.0f); glVertex3f(1.0,1.0,-1.0);
            glTexCoord2f(1.0f,0.0f); glVertex3f(-1.0,1.0,-1.0);
        }
        glEnd();
    }
    glEndList();
}

void RigidBodyRendering::cleanup()
{
    glDeleteTextures(1, &boxTexture);
    glDeleteLists(boxList, 1);
}

void RigidBodyRendering::renderBox()
{
    glDisable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, boxTexture);
    glCallList(boxList);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);
}

void RigidBodyRendering::renderSphere()
{

}

void RigidBodyRendering::renderBoat()
{

}
