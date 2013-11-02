#ifndef UTILS_H
#define UTILS_H

#include <cmath>

class Utils
{
public:
    Utils();

    static float distance(float x1, float y1, float x2, float y2);
    static void adjustCoordinates3DToScene(float& x, float& y, float width, float height);
    static void adjustCoordinatesSceneTo3D(float& x, float& y, float width, float height);

    static void normalize(float& n_x, float& n_y);
    static void normalize(float& n_x, float& n_y, float& n_z);
    static float dotProduct(float x1, float y1, float x2, float y2);

    static float getZValueForeground();
    static float getZValueBackground();

    static void crossProduct(float x1, float y1, float z1, float x2, float y2, float z2, float& x, float& y, float& z);
};

#endif // UTILS_H
