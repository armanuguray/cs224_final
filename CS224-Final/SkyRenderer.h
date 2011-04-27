#ifndef SKYRENDERER_H
#define SKYRENDERER_H

#include "OpenGLInclude.h"

class Camera;

class SkyRenderer
{
public:
    SkyRenderer();
    ~SkyRenderer();

    inline GLuint getTexture() { return skytexture; }
    void renderSkyBox(Camera *camera);

private:
    GLuint skybox, skytexture;
};

#endif // SKYRENDERER_H
