#ifndef OPENGLINCLUDE_H
#define OPENGLINCLUDE_H

#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES

    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glext.h>
    #include <GL/glut.h>
    #include <GL/freeglut.h>

extern "C" {
    void APIENTRY glActiveTexture(GLenum);
    extern void APIENTRY glActiveTexture(GLenum);
    extern void APIENTRY glMultiTexCoord3f(GLenum, GLfloat, GLfloat, GLfloat);
    extern void APIENTRY glMultiTexCoord4f(GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
}

#endif

#endif // OPENGLINCLUDE_H
