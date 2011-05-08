#include "RigidBodyRendering.h"
#include "GLFileLoader.h"
#include "RigidBodyConstants.h"

GLuint boxTexture = 0;
GLuint boxList = 0;

void RigidBodyRendering::initialize()
{
    // load textures
    QString s(":/crate");
    GLFileLoader::loadTexture2D(s, boxTexture);

    const float extent = SIDE_LENGTH/2.0f;

    // load lists
    boxList = glGenLists(1);
    glNewList(boxList, GL_COMPILE);
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        {
            glNormal3f(0.0, 0.0, 1.0);
            glTexCoord2f(1.0f,1.0f); glVertex3f(extent,extent,extent);
            glTexCoord2f(0.0f,1.0f); glVertex3f(-extent,extent,extent);
            glTexCoord2f(0.0f,0.0f); glVertex3f(-extent,-extent,extent);
            glTexCoord2f(1.0f,0.0f); glVertex3f(extent,-extent,extent);

            glNormal3f(-1.0, 0.0, 0.0);
            glTexCoord2f(1.0f,1.0f); glVertex3f(-extent,extent,extent);
            glTexCoord2f(0.0f,1.0f); glVertex3f(-extent,extent,-extent);
            glTexCoord2f(0.0f,0.0f); glVertex3f(-extent,-extent,-extent);
            glTexCoord2f(1.0f,0.0f); glVertex3f(-extent,-extent,extent);

            glNormal3f(0.0, 0.0, -1.0);
            glTexCoord2f(1.0f,1.0f); glVertex3f(-extent,extent,-extent);
            glTexCoord2f(0.0f,1.0f); glVertex3f(extent,extent,-extent);
            glTexCoord2f(0.0f,0.0f); glVertex3f(extent,-extent,-extent);
            glTexCoord2f(1.0f,0.0f); glVertex3f(-extent,-extent,-extent);

            glNormal3f(1.0, 0.0, 0.0);
            glTexCoord2f(1.0f,1.0f); glVertex3f(extent,extent,-extent);
            glTexCoord2f(0.0f,1.0f); glVertex3f(extent,extent,extent);
            glTexCoord2f(0.0f,0.0f); glVertex3f(extent,-extent,extent);
            glTexCoord2f(1.0f,0.0f); glVertex3f(extent,-extent,-extent);

            glNormal3f(0.0, -1.0, 0.0);
            glTexCoord2f(1.0f,1.0f); glVertex3f(extent,-extent,-extent);
            glTexCoord2f(0.0f,1.0f); glVertex3f(extent,-extent,extent);
            glTexCoord2f(0.0f,0.0f); glVertex3f(-extent,-extent,extent);
            glTexCoord2f(1.0f,0.0f); glVertex3f(-extent,-extent,-extent);

            glNormal3f(0.0, 1.0, 0.0);
            glTexCoord2f(1.0f,1.0f); glVertex3f(-extent,extent,extent);
            glTexCoord2f(0.0f,1.0f); glVertex3f(extent,extent,extent);
            glTexCoord2f(0.0f,0.0f); glVertex3f(extent,extent,-extent);
            glTexCoord2f(1.0f,0.0f); glVertex3f(-extent,extent,-extent);
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

void RigidBodyRendering::boxPerFacePass()
{

}

void RigidBodyRendering::renderSphere()
{

}

void RigidBodyRendering::renderBoat()
{

}
