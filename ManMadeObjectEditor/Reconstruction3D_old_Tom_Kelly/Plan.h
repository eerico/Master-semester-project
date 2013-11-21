#ifndef PLAN_H
#define PLAN_H

#include "Intersection.h"
#include "../Geometry/Vertex.h"
#include "../Geometry/Profile.h"

class Plan
{
public:
    Plan(float pointX, float pointY, float pointZ,
         float normalX = 0.0f, float normalY = 0.0f, float normalZ = 1.0f);

    Intersection intersect3Plans(Plan& plan2, Plan& plan3);

    void setNormal(float nx, float ny, float nz);
    void computePlanNormal(Vertex* vertex1, Vertex* vertex2, Profile* profile);

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
