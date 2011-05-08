/**
 * @author auguray
 * @paragraph Utility functions used to render various rigid body shapes that are used in the simulation.
 */

#ifndef RIGIDBODYRENDERING_H
#define RIGIDBODYRENDERING_H

#include "OpenGLInclude.h"

namespace RigidBodyRendering
{
    // initialize should be called to initialize rendering code (textures, lists etc)
    void initialize();
    void cleanup();

    /* box */
    // renders a texture mapped box, dowloading texture coordinates, normals and vertex data to the GPU
    void renderBox();
    // renders each face as a point primitive. Normal and vertex data are passed in as vertex attributes.
    void boxPerFacePass();

    void renderSphere();
    void renderBoat();
};

#endif // RIGIDBODYRENDERING_H
