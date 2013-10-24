#include "Utils.h"

Utils::Utils()
{
}

float Utils::distance(float x1, float y1, float x2, float y2)
{
    float diffX = x1 - x2;
    float diffY = y1 - y2;
    return std::sqrt(diffX * diffX + diffY * diffY);
}

void Utils::adjustCoordinates3DToScene(float& x, float& y, float width, float height)
{
    // from 3D to Scene coord
    float width_2 = width/ 2.0f;
    float height_2 = height / 2.0f;

    x = x * width_2;
    y = y * width_2;

    y = height_2 - y;
    x = x + width_2;
}

void Utils::adjustCoordinatesSceneTo3D(float& x, float& y, float width, float height)
{
    // from scene coord to 3D
    float width_2 = width/ 2.0f;
    float height_2 = height / 2.0f;

    y = height_2  - y;
    x = -width_2 + x;

    x = x/width_2;
    y = y/width_2;
}

void Utils::normalize(float &n_x, float &n_y)
{
    float distTmp = n_x * n_x + n_y * n_y;
    if (distTmp != 0.0f) {
        distTmp = sqrt(distTmp);
        n_x = n_x / distTmp;
        n_y = n_y / distTmp;
    }
}

float Utils::dotProduct(float x1, float y1, float x2, float y2)
{
    return x1 * x2 + y1 * y2;
}

float Utils::getZValueForeground()
{
    return 1.0f;
}

float Utils::getZValueBackground()
{
    return 0.0f;
}
