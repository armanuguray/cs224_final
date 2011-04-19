#include "ProjectorCamera.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <glu.h>
#endif

#include <float.h>

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
    ProjectorCamera::loadMatrices();
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
}

void ProjectorCamera::renderProjectedGrid()
{
    if (grid_visible)
    {
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
    }
}


