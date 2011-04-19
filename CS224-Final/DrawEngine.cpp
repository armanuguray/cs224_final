#include "DrawEngine.h"
#include "ProjectorCamera.h"
#include <QGLShaderProgram>

DrawEngine::DrawEngine(int width, int height)
{
    setupGL();
    m_skyrenderer = NULL;
    m_projectorcamera = new ProjectorCamera(width, height);
}

DrawEngine::~DrawEngine()
{
    delete m_skyrenderer;
    delete m_projectorcamera;
    for (std::map<string, QGLShaderProgram *>::iterator it = m_shaderprograms.begin(); it != m_shaderprograms.end(); ++it) {
        delete it->second;
    }
}

void DrawEngine::resize(REAL width, REAL height)
{
    m_projectorcamera->resize(width, height);
}

void DrawEngine::setupGL()
{
    glClearColor(0.0, 0.0, 0.0, 0);
    glFrontFace(GL_CCW);
    glDisable(GL_DITHER);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

}

void DrawEngine::drawFrame()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    m_projectorcamera->renderProjectedGrid();
}

void DrawEngine::mouse_down(Vector2 &mouse_pos)
{

}

void DrawEngine::mouse_scroll(REAL delta)
{
    m_projectorcamera->lookVectorTranslate(delta);
}

void DrawEngine::mouse_dragged(Vector2 &delta, MouseButton button)
{
    switch (button)
    {
        case MouseButtonLeft:
            m_projectorcamera->lookVectorRotate(delta);
            break;
        case MouseButtonRight:
            m_projectorcamera->filmPlaneTranslate(delta);
            break;
    }
}
