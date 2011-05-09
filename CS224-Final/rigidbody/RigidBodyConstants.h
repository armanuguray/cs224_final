#ifndef RIGIDBODYCONSTANTS_H
#define RIGIDBODYCONSTANTS_H

#include <cmath>

/* NOTE: I'm using SI units to be consistent but the values here make no sense in real life. I mean, a 2000 ton create that floats? Come on. */

// The maximum number of rigid bodies to pre-allocate for the entire simulation
const unsigned RIGIDBODY_MAX_COUNT = 1000;
// internal number of substep for each step of the physics simulation
const int MAX_SUBSTEPS = 10;
// resolution of the buoyancy image. this should be relatively low for performance reasons
const unsigned BUOYANCY_IMAGE_RESOLUTION = 16;
// impulse scale factor shooting boxes
const float IMPULSE_SCALE = 100000.0f;
// water density (kg/m^2)
const float WATER_DENSITY = 1000.0f;
// clip height (unused)
const float MAX_ABS_HEIGHT = 1000.0f;
// mass of a crate (kg)
const float BOX_MASS = 6000.f;
// gravitational acceleration (m/s^2)
const float GRAVITY = 9.8f;
// length of one side of an object bounding box
const float SIDE_LENGTH = 2.0f;
// per object orthgonal view extent
const float OBJ_EXTENT = sqrt((SIDE_LENGTH*SIDE_LENGTH)+(SIDE_LENGTH*SIDE_LENGTH)) + 0.5;
// lift and drag coefficients
const float LIFT_COEFFICIENT = 1.3;
const float DRAG_COEFFICIENT = 2.1;
// epsilon value that defines the dependency of face area on direction of motion. ([0, 1])
const float LD_EPSILON = 0.3;

#endif // RIGIDBODYCONSTANTS_H
