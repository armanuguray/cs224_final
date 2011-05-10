#include "Settings.h"

Settings settings;

Settings::Settings()
{
    grid_resolution = 600;
    line_mode = false;
    dv_scale = 1.4;
    ambient_waves = false;

    tvs = false;
    ow_waves = false;

    render_overlay = false;
    overlay_text_offset = QPoint(10, 10);
    overlay_text_max_border = 10;
    overlay_text_font = QFont("Sans", 10);
    overlay_text = QObject::tr("ESC: Exit fullscreen mode\n"
                               "F11: Toggle fullscreen\n"
                               "F: Toggle fullscreen\n"
                               "A: Toggle ambient waves\n"
                               "S: Show/hide debugging information");
}
