#ifndef SETTINGS_H
#define SETTINGS_H

struct Settings
{
    Settings();
    unsigned grid_resolution;
    bool line_mode;
};

extern Settings settings;

#endif // SETTINGS_H
