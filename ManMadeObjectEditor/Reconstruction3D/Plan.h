#ifndef PLAN_H
#define PLAN_H

#include "GeneralEvent.h"
#include "../Geometry/Vertex.h"
#include "../Geometry/Profile.h"
#include "../GarbageCollector/GarbageCollectorObject.h"
#include "../GarbageCollector/GeneralDestructorManager.h"

/**
 * @brief The Plan class
 * This class represent the direction plan associated with the edges.
 * A direction plan is defined by the edge and the profile.
 */
class Plan: public GarbageCollectorObject
{
public:
    /**
     * @brief Plan
     * Compute a direction plan with the two vertices (of an edge). The normal can then be computed by calling
     * the computePlanNormal method.
     * @param vertex1
     * @param vertex2
     * @param profile
     */
    Plan(Vertex* vertex1, Vertex* vertex2, Profile* profile);

    /**
     * @brief Plan
     * Compute a direction plan based on an old direction plan
     * @param oldPlan
     * @param vertex1
     * @param profile
     */
    Plan(Plan* oldPlan, Vertex* vertex1, Profile* profile);

    /**
     * @brief Plan
     * Compute a direction plan for the horizontal plan
     * @param vertexX
     * @param vertexY
     * @param vertexZ
     * @param normalX
     * @param normalY
     * @param normalZ
     */
    Plan(float vertexX, float vertexY, float vertexZ,
         float normalX = 0.0f, float normalY = 0.0f, float normalZ = 1.0f);

    ~Plan();

    /**
     * @brief intersect3Plans
     * Intersect three different plan
     * @param plan2
     * @param plan3
     * @return the intersection or 0 if it failed
     */
    GeneralEvent* intersect3Plans(Plan *plan2, Plan *plan3);

    void setNormal(float nx, float ny, float nz);
    void getNormal(float& nx, float& ny, float& nz);

    /**
     * @brief computePlanNormal
     * Compute the normal of this direction plan if possible
     * (one of this constructor has been used:
     * Plan(Vertex* vertex1, Vertex* vertex2, Profile* profile);
     * Plan(Plan* oldPlan, Vertex* vertex1, Profile* profile);
     * )
     */
    void computePlanNormal();

    void setVertex(Vertex* vertex);

    Vertex* getVertex();

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

    // the profile associated with this direction plan
    Profile* profile;
};

#endif // PLAN_H
