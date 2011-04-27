/**
 * @author auguray
 * convenience methods for loading, textures and shaders (for those who don't like QGLShaderProgram).
 */

#ifndef GLFILELOADER_H
#define GLFILELOADER_H

#include "OpenGLInclude.h"
#include <QString>

namespace GLFileLoader
{
    /**
     * @return true if the texture loaded successfully, false otherwise.
     *
     * Inputs:
     * @param filename: name of the texture file, input
     *
     * Outputs:
     * @param texture_id: the generated texture id of the texture object
     */
    extern bool loadTexture2D(QString &filename, GLuint &texture_id);

    /**
     * @return true if the cube map loaded successfully, false otherwise.
     *
     * Inputs:
     * @param filenames for the cube map
     *
     * Outputs:
     * @param texture_id: the generated texture id of the texture object
     */
    extern bool loadCubeMap(QString filenames[6], GLuint &texture_id);
}

#endif // GLFILELOADER_H
