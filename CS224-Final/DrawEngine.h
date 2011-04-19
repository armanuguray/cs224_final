#ifndef DRAWENGINE_H
#define DRAWENGINE_H

#include "OpenGLInclude.h"
#include "CS123Algebra.h"
#include <map>
#include <string>

class QGLShaderProgram;
class ProjectorCamera;
class SkyRenderer;

typedef enum {
    MouseButtonLeft,
    MouseButtonRight
} MouseButton;

class DrawEngine
{
public:
    DrawEngine(int width, int height);
    ~DrawEngine();

    // does initial OpenGL setup
    void setupGL();
    // draws to the rendering context. Should be called each frame
    void drawFrame();
    // should be called when the window gets resized
    void resize(REAL width, REAL height);

    // mouse interaction
    void mouse_down(Vector2 &mouse_pos);
    void mouse_scroll(REAL delta);
    void mouse_dragged(Vector2 &delta, MouseButton button);

protected:
    SkyRenderer *m_skyrenderer; // handles the rendering of the sky, including the sun.
    ProjectorCamera *m_projectorcamera; // represents the OpenGL camera. Also used for rendering the projected grid

    std::map<string, QGLShaderProgram *> m_shaderprograms; // maps a shader program object to a given name.
};

#endif // DRAWENGINE_H
