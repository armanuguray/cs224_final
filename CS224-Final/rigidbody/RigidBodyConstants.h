#ifndef RIGIDBODYCONSTANTS_H
#define RIGIDBODYCONSTANTS_H

// The maximum number of rigid bodies to pre-allocate for the entire simulation
const unsigned RIGIDBODY_MAX_COUNT = 1000;
// resolution of the buoyancy image. this should be relatively low for performance reasons
const unsigned BUOYANCY_IMAGE_RESOLUTION = 16;
// water density
const float WATER_DENSITY = 1.0f;
// clip height
const float MAX_ABS_HEIGHT = 400.0f;

#endif // RIGIDBODYCONSTANTS_H
