#ifndef PLAN_H
#define PLAN_H

#include "Intersection.h"

class Plan
{
public:
    Plan(const float& pointX, const float& pointY, const float& pointZ,
         const float& normalX = 0.0f, const float& normalY = 0.0f, const float& normalZ = 1.0f);

    Intersection intersect3Plans(const Plan& plan2, const Plan& plan3);

    void setNormal(const float& nx, const float& ny, const float& nz);

private:
    // a plan is defined by a point and a normal
    float pointX;
    float pointY;
    float pointZ;

    float normalX;
    float normalY;
    float normalZ;
};

#endif // PLAN_H
