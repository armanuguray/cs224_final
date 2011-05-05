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

    void renderBox();
    void renderSphere();
    void renderBoat();
};

#endif // RIGIDBODYRENDERING_H
