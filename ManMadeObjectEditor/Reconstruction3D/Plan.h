#ifndef PLAN_H
#define PLAN_H

#include "GeneralEvent.h"
#include "../Geometry/Vertex.h"
#include "../Geometry/Profile.h"
#include "../GarbageCollector/GarbageCollectorObject.h"
#include "../GarbageCollector/GeneralDestructorManager.h"

class Plan: public GarbageCollectorObject
{
public:
    Plan(Vertex* vertex1, Vertex* vertex2, Profile* profile);

    Plan(Plan* oldPlan, Vertex* vertex1, Profile* profile);

    Plan(float vertexX, float vertexY, float vertexZ,
         float normalX = 0.0f, float normalY = 0.0f, float normalZ = 1.0f);

    ~Plan();

    GeneralEvent* intersect3Plans(Plan *plan2, Plan *plan3);

    void setNormal(float nx, float ny, float nz);
    void getNormal(float& nx, float& ny, float& nz);
    void computePlanNormal();

    void setVertex(Vertex* vertex);

private:
    // a plan is defined by a point and a normal
    Vertex* vertex;

    float normalX;
    float normalY;
    float normalZ;

    // information used to compute the normal: the coordinates of the two vertices at the first level
    float pointX2;
    float pointY2;
    float pointZ2;

    float pointX;
    float pointY;
    float pointZ;

    Profile* profile;
};

#endif // PLAN_H
