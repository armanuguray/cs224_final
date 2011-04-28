#ifndef OPENGLINCLUDE_H
#define OPENGLINCLUDE_H

#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#define GL_GLEXT_PROTOTYPES
extern "C" {
    void APIENTRY glActiveTexture(GLenum);
}
#endif

#endif // OPENGLINCLUDE_H
