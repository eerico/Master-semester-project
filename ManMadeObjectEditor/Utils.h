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
    static float dotProduct(float x1, float y1, float x2, float y2);

    static float getZValueForeground();
    static float getZValueBackground();
};

#endif // UTILS_H
