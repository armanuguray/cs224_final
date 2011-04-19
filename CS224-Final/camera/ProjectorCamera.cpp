#include "ProjectorCamera.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <glu.h>
#endif

#include <float.h>
#include "Settings.h"

#define CORNER_LINES

// given points v0 and v1 and an x value between v0.x and v1.x, returns the linearly interpolated y value
void lerp(Vector2 v0, Vector2 v1, REAL x, REAL &y)
{
    y = v0.y + ((x - v0.x) * v1.y - (x - v0.x) * v0.y)/(v1.x - v0.x);
}

bool ProjectorCamera::intersectSegmentPlane(Vector4 v1, Vector4 v2, REAL y, Vector4 &intersect)
{
    Vector4 dir = v2 - v1;
    REAL dist = dir.getMagnitude();
    dir = dir.getNormalized();
    if (dir.y >= DBL_EPSILON || dir.y <= -DBL_EPSILON) {
        REAL t = (y-v1.y)/dir.y;
        dir *= t;
        if (t >= 0 && dir.getMagnitude() <= dist) {
            intersect = v1 + dir;
            return true;
        }
    }
    return false;
}

ProjectorCamera::ProjectorCamera(int width, int height) : Camera(width, height)
{
    left_points = right_points = NULL;
    ProjectorCamera::loadMatrices();
}

ProjectorCamera::~ProjectorCamera()
{
    delete[] left_points;
    delete[] right_points;
}

void ProjectorCamera::loadMatrices()
{
    Camera::loadMatrices();
    Matrix4x4 viewproj = (Matrix4x4(m_modelview_matrix) * Matrix4x4(m_projection_matrix)).getTranspose();
    Matrix4x4 inv_viewproj = viewproj.getInverse();

    Vector4 *corners = new Vector4[8];
    // Near plane (at z = 0)
    corners[0].x = -1, corners[0].y = -1, corners[0].z = 0, corners[0].w = 1;
    corners[1].x =  1, corners[1].y = -1, corners[1].z = 0, corners[1].w = 1;
    corners[2].x = -1, corners[2].y =  1, corners[2].z = 0, corners[2].w = 1;
    corners[3].x =  1, corners[3].y =  1, corners[3].z = 0, corners[3].w = 1;
    // Far plane (should be at z = 1)
    corners[4].x = -1, corners[4].y = -1, corners[4].z = 1, corners[4].w = 1;
    corners[5].x =  1, corners[5].y = -1, corners[5].z = 1, corners[5].w = 1;
    corners[6].x = -1, corners[6].y =  1, corners[6].z = 1, corners[6].w = 1;
    corners[7].x =  1, corners[7].y =  1, corners[7].z = 1, corners[7].w = 1;

    // transform to world space
    Vector4 v;
    for (int i = 0; i < 8; i++) {
        v = corners[i];
        corners[i] = inv_viewproj * v;
        corners[i].homogenize();
    }

    // get intersection points between the edges of
    // the camera frustum and the displacable volume
    std::list<Vector4> intersections;
    const REAL delta = 0.0;//settings.getAmplitudeDelta();
    for (int i = 0; i < 4; i++) {
        if (intersectSegmentPlane(corners[i], corners[i+4], delta, v)) {
            v.y = 0; // project the point onto S_base
            intersections.push_front(v);
        }
        // don't intersect twice if delta is 0
        if (delta != 0 && intersectSegmentPlane(corners[i], corners[i+4], -delta, v)) {
            v.y = 0; // project the point onto S_base
            intersections.push_front(v);
        }
    }

    /*
     * Intersect the edges of the far plane
     * The edge vertex indices are as follows:
     *
     *      6----------7
     *      |          |
     *      |          |
     *      |          |
     *      4----------5
     */
    for (int i = 4; i < 8; i += 3) {
        for (int j = 5; j < 7; j++) {
            if (intersectSegmentPlane(corners[i], corners[j], delta, v)) {
                v.y = 0; // project the point onto S_base
                intersections.push_front(v);
            }
            if (delta != 0 && intersectSegmentPlane(corners[i], corners[j], -delta, v)) {
                v.y = 0; // project the point onto S_base
                intersections.push_front(v);
            }
        }
    }

    /*
     * Intersect the edges of the near plane
     * The edge vertex indices are as follows:
     *
     *      2----------3
     *      |          |
     *      |          |
     *      |          |
     *      0----------1
     */
    for (int i = 0; i < 4; i += 3) {
        for (int j = 1; j < 3; j++) {
            if (intersectSegmentPlane(corners[i], corners[j], delta, v)) {
                v.y = 0; // project the point onto S_base
                intersections.push_front(v);
            }
            if (delta != 0 && intersectSegmentPlane(corners[i], corners[j], -delta, v)) {
                v.y = 0; // project the point onto S_base
                intersections.push_front(v);
            }
        }
    }

    // check if any of the corner points lie inside the volume and add them as well
    if (delta != 0) {
        for (int i = 0; i < 8; i++) {
            v = corners[i];
            if (v.y > -delta && v.y < delta) {
                v.y = 0;
                intersections.push_front(v);
            }
        }
    }

    // if no intersections are found, return false so that the surface isn't rendered
    if (intersections.size() == 0) {
        logln("NO");
        delete corners;
        grid_visible = false;
        return;
    }
    logln("YES");
    grid_visible = true;


    // find the corner points of the grid
    // TODO: TEST THIS!!
    Vector4 center;
    for (std::list<Vector4>::iterator it = intersections.begin(); it != intersections.end(); ++it) {
        center += (viewproj * (*it)).getHomogenized();
    }
    center /= intersections.size();
    Vector4 lli,lri,uli,uri;
    lri = uli = uri = lli = center;
    for (std::list<Vector4>::iterator it = intersections.begin(); it != intersections.end(); ++it) {
        v = viewproj * (*it);
        v.homogenize();
        if (v.x < center.x && v.y < lli.y) {
            lli = v;
            ll = (*it);
        } else if (v.x > center.x && v.y > uri.y) {
            uri = v;
            ur = (*it);
        } else if (v.x < center.x && v.y > uli.y) {
            uli = v;
            ul = (*it);
        } else if (v.x > center.x && v.y < lri.y) {
            lri = v;
            lr = (*it);
        }
    }
    logln(intersections.size());
    logln("RESULTS: \n");
    logln(lli);
    logln(lri);
    logln(uli);
    logln(uri);
    delete corners;

    // store the left and right sides
    delete[] left_points;
    left_points = new Vector4[settings.grid_resolution + 1];
    delete[] right_points;
    right_points = new Vector4[settings.grid_resolution + 1];

    Vector4 screen_left = viewproj * ul;
    screen_left.homogenize();
    Vector4 screen_right = viewproj * ur;
    screen_right.homogenize();

    Vector4 left_dir = (viewproj * ll).homogenize() - screen_left;
    left_dir.unhomgenize();
    left_dir = left_dir.getNormalized() * (left_dir.getMagnitude()/static_cast<REAL>(settings.grid_resolution));

    Vector4 right_dir = (viewproj * lr).homogenize() - screen_right;
    right_dir.unhomgenize();
    right_dir = right_dir.getNormalized() * (right_dir.getMagnitude()/static_cast<REAL>(settings.grid_resolution));

    Vector4 near, far;
    for (int i = 0; i <= settings.grid_resolution; i++) {
        far = (inv_viewproj * Vector4(screen_left.x, screen_left.y, 1, 1)).homogenize();
        near = (inv_viewproj * Vector4(screen_left.x, screen_left.y, 0, 1)).homogenize();
        intersectSegmentPlane(near, far, 0, v);
        left_points[i] = v;
        screen_left += left_dir;

        far = (inv_viewproj * Vector4(screen_right.x, screen_right.y, 1, 1)).homogenize();
        near = (inv_viewproj * Vector4(screen_right.x, screen_right.y, 0, 1)).homogenize();
        intersectSegmentPlane(near, far, 0, v);
        right_points[i] = v;
        screen_right += right_dir;
    }
}

void ProjectorCamera::renderProjectedGrid()
{
    if (grid_visible)
    {
#undef CORNER_LINES
#ifdef CORNER_LINES
        glBegin(GL_LINES);
        glColor3d(1, 0, 0);
        glVertex3d(ll.x, ll.y, ll.z);
        glVertex3d(ur.x, ur.y, ur.z);
        glColor3d(0, 1, 0);
        glVertex3d(lr.x, lr.y, lr.z);
        glVertex3d(ul.x, ul.y, ul.z);
        glColor3d(0, 1, 1);
        glVertex3d(lr.x, lr.y, lr.z);
        glVertex3d(ll.x, ll.y, ll.z);
        glColor3d(1, 1, 0);
        glVertex3d(ur.x, ur.y, ur.z);
        glVertex3d(ul.x, ul.y, ul.z);
        glEnd();
#endif

        glColor3f(0, 1, 1);

        if (settings.line_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        Vector2 v0curr, v1curr, v0next, v1next;
        Vector2 xdircurr, xdirnext;
        Vector2 vcurr, vnext;
        for (unsigned i = 0; i < settings.grid_resolution; i++) {
            v0curr = Vector2(left_points[i].x, left_points[i].z);
            v1curr = Vector2(right_points[i].x, right_points[i].z);
            v0next = Vector2(left_points[i+1].x, left_points[i+1].z);
            v1next = Vector2(right_points[i+1].x, right_points[i+1].z);
            vcurr = v0curr;
            vnext = v0next;
            xdircurr = v1curr - v0curr;
            xdircurr = xdircurr.getNormalized() * (xdircurr.getMagnitude()/static_cast<REAL>(settings.grid_resolution));
            xdirnext = v1next - v0next;
            xdirnext = xdirnext.getNormalized() * (xdirnext.getMagnitude()/static_cast<REAL>(settings.grid_resolution));
            glBegin(GL_QUAD_STRIP);
            for (unsigned i = 0; i <= settings.grid_resolution; i++) {
                glVertex3f(vcurr.x, 0, vcurr.y);
                glVertex3f(vnext.x, 0, vnext.y);
                vcurr += xdircurr;
                vnext += xdirnext;
            }
            glEnd();
        }
    }
}


