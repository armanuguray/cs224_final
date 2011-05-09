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

    void update(float time_elapsed);

    void generateUniformWave(int numParticles, const Vector2 &origin, float amplitude);

    /**
     * Renders all particles onto the screen as spheres. This is pretty much just for testing.
     */
    void drawParticlesAsSpheres(GLUquadric *quadric);

    QSet<WaveParticle *>& liveParticles() { return m_liveParticles; }

    void load(const QGLContext *context);
    void unload();

    void moveHeightmap(ProjectorCamera *camera);

    void renderHeightmap();
    void blurHeightmap();
    void renderWaves(ProjectorCamera* camera, SkyRenderer *sky);

    void debugDrawHeightmap();

    void draw(ProjectorCamera* camera, SkyRenderer *sky);

private:
    void loadShaders(const QGLContext *context);
    void createFbos();
    void computeWeights();

    Pool m_particleStore;
    QSet<WaveParticle *> m_liveParticles;

    std::map<string, QGLShaderProgram *> m_shaderprograms;
    std::map<string, QGLFramebufferObject *> m_fbos;

    float m_heightmapX, m_heightmapZ;

    float _weightsX[3 * WAVE_CONVOLUTION_KERNEL_WIDTH];
    float _weightsZ[3 * WAVE_CONVOLUTION_KERNEL_WIDTH];

};

#endif // WAVEPARTICLEMANAGER_H
