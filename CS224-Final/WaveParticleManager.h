#ifndef WAVEPARTICLEMANAGER_H
#define WAVEPARTICLEMANAGER_H

#include <map>

#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include <QSet>

#include "Pool.h"
#include "ProjectorCamera.h"
#include "SkyRenderer.h"
#include "WaveConstants.h"
#include "WaveParticle.h"

class WaveParticleManager
{
public:
    WaveParticleManager();
    virtual ~WaveParticleManager();

    void update(float time, float time_elapsed);

    void generateUniformWave(int numParticles, const Vector2 &origin, float amplitude, float time);

    /**
     * Renders all particles onto the screen as spheres. This is pretty much just for testing.
     */
    void drawParticlesAsSpheres(GLUquadric *quadric);

    QSet<WaveParticle *>& liveParticles() { return m_liveParticles; }

    void load(const QGLContext *context);
    void unload();

    void moveHeightmap(ProjectorCamera *camera);

    void renderHeightmap(float t);
    void blurHeightmap();
    void renderWaves(ProjectorCamera* camera, SkyRenderer *sky);

    void debugDrawHeightmap();

    void draw(ProjectorCamera* camera, SkyRenderer *sky, float t);

    GLuint heightTexture()   { return m_heightVelocityTargets[0]; }    // G = height
    GLuint velocityTexture() { return m_heightVelocityTargets[1]; }    // R = x, G = y

    float heightmapOriginX() { return m_heightmapX; }
    float heightmapOriginZ() { return m_heightmapZ; }

private:
    void loadShaders(const QGLContext *context);
    void createFbos();
    void computeWeights();

    Pool m_particleStore;
    QSet<WaveParticle *> m_liveParticles;

    std::map<string, QGLShaderProgram *> m_shaderprograms;

    GLuint m_heightVelocityFBO;
    GLuint m_heightVelocityTargets[2]; // [0] is 3D heightmap (RGB32), [1] is 2D velocity (RG16)
    GLuint m_convolutionFBO;
    GLuint m_convolutionTargets[2];

    float m_heightmapX, m_heightmapZ;

    float _heightWeightsX[3 * WAVE_CONVOLUTION_KERNEL_WIDTH];
    float _heightWeightsZ[3 * WAVE_CONVOLUTION_KERNEL_WIDTH];
    float _velocityWeightsX[2 * WAVE_CONVOLUTION_KERNEL_WIDTH];
    float _velocityWeightsZ[2 * WAVE_CONVOLUTION_KERNEL_WIDTH];
};

#endif // WAVEPARTICLEMANAGER_H
