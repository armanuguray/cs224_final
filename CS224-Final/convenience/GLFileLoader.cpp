#include "GLFileLoader.h"
#include <QImage>
#include <iostream>
#include <QGLWidget>

bool GLFileLoader::loadTexture2D(QString &filename, GLuint &texture_id)
{
    QImage image, texture;
    if(!image.load(filename)) {
        std::cerr << "Failed to load image from file named: " << filename.toStdString() << std::endl;
        return false;
    }
    image = image.mirrored(false, true);
    texture = QGLWidget::convertToGLFormat(image);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 3, 3, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "Error loading \"" << filename.toStdString() <<  "\" to VRAM." << std::endl;
        glDeleteTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "Error loading mipmaps for \"" << filename.toStdString() <<  "\"." << std::endl;
        glDeleteTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

bool GLFileLoader::loadCubeMap(QString filenames[6], GLuint &texture_id)
{
    QImage images[6], textures[6];
    for (int i = 0; i < 6; i++) {
        if (!images[i].load(filenames[i])) {
            std::cerr << "Failed to load image from file named: " << filenames[i].toStdString() << std::endl;
            return false;
        }
        images[i] = images[i].mirrored(false, true);
        textures[i] = QGLWidget::convertToGLFormat(images[i]);
    }
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLenum err;
    for (unsigned i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 3, 3, textures[i].width(), textures[i].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textures[i].bits());
        if ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "Error loading \"" << filenames[i].toStdString() <<  "\" to VRAM." << std::endl;
            glDeleteTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            return false;
        }
        gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 3, textures[i].width(), textures->height(), GL_RGBA, GL_UNSIGNED_BYTE, textures[i].bits());
        if ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "Error loading mipmaps for \"" << filenames[i].toStdString() <<  "\"." << std::endl;
            glDeleteTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            return false;
        }
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return true;
}

