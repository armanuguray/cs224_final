#ifndef PROJECTORCAMERA_H
#define PROJECTORCAMERA_H

#include "Camera.h"

class ProjectorCamera : public Camera
{
public:
    ProjectorCamera(int width, int height);
    ~ProjectorCamera();

    // TODO: make this render the water grid using interpolation if grid_visible == true
    void renderProjectedGrid();

    // intersects a line segment with a plane that is perpendicular to the y-axis
    static bool intersectSegmentPlane(Vector4 v1, Vector4 v2, REAL y, Vector4 &intersect);

    Vector4 ll,lr,ul,ur; // corner points of the grid

private:

    virtual void loadMatrices();

    Vector4 *left_points;
    Vector4 *right_points;

    bool grid_visible;
};

#endif // PROJECTORCAMERA_H
