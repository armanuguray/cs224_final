#ifndef OPENGLINCLUDE_H
#define OPENGLINCLUDE_H

#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glext.h>
    #include <GL/glut.h>
    #include <GL/freeglut.h>
#define GL_GLEXT_PROTOTYPES
extern "C" {
    void APIENTRY glActiveTexture(GLenum);
}
#endif


#endif // OPENGLINCLUDE_H
