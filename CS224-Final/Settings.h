#ifndef SETTINGS_H
#define SETTINGS_H

#include "CS123Algebra.h"

struct Settings
{
    Settings();
    unsigned grid_resolution;
    bool line_mode;

    /**
     * This defines a scale value for the projected grid computation. To account for the changes in the visible
     * portion of the water plane, when height field displacements would otherwise cause unwanted artifacts on the projected
     * grid, we can modify the "projector frustum" by scaling the field of view. This creates a larger grid than what would be generated
     * if only the camera frustum were used for the projection.
     *
     * "dv" stands for "displaceable volume", taken from the projected grid paper.
     */
    REAL dv_scale;
};

extern Settings settings;

#endif // SETTINGS_H
