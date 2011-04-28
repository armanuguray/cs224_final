#include "Settings.h"

Settings settings;

Settings::Settings()
{
    grid_resolution = 40;
    line_mode = false;
    dv_scale = 1.4;

    overlay_text_offset = QPoint(10, 10);
    overlay_text_max_border = 10;
    overlay_text_font = QFont("Sans", 10);
    overlay_text = QObject::tr("ESC: Exit fullscreen mode\n"
                               "F11: Toggle fullscreen\n"
                               "F: Toggle fullscreen\n"
                               "S: Show/hide debugging information");
}
