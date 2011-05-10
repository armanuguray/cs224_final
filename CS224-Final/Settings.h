#ifndef SETTINGS_H
#define SETTINGS_H

#include <QFont>
#include <QObject>
#include <QPoint>
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

    /**
     * Values relating to the position of the overlay text
     */
    QPoint overlay_text_offset;
    float overlay_text_max_border;
    QFont overlay_text_font;
    QString overlay_text;

    bool render_overlay;

    bool ambient_waves;
};

extern Settings settings;

#endif // SETTINGS_H
