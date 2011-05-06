#ifndef RIGIDBODYCONSTANTS_H
#define RIGIDBODYCONSTANTS_H

// The maximum number of rigid bodies to pre-allocate for the entire simulation
const unsigned RIGIDBODY_MAX_COUNT = 1000;
// resolution of the buoyancy image. this should be relatively low for performance reasons
const unsigned BUOYANCY_IMAGE_RESOLUTION = 16;
// water density (kg/m^2)
const float WATER_DENSITY = 1000.0f;
// clip height
const float MAX_ABS_HEIGHT = 1000.0f;
// mass of a crate
const float BOX_MASS = 200.f;
// gravitational acceleration (m/s^2)
const float GRAVITY = 10.f;

#endif // RIGIDBODYCONSTANTS_H
