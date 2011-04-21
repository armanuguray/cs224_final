#ifndef SETTINGS_H
#define SETTINGS_H

#include "CS123Algebra.h"

struct Settings
{
    Settings();
    unsigned grid_resolution;
    bool line_mode;
    REAL dv_delta; // displaceable volume delta
};

extern Settings settings;

#endif // SETTINGS_H
