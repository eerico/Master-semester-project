#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H
#include <vector>
#include "../Geometry/Edge.h"
#include "../Geometry/Vertex.h"
#include "../Geometry/Profile.h"

class ActivePlan
{
public:
    ActivePlan(Vertex* floorPlan, unsigned int floorPlanSize);
    std::vector< Edge* >* getPlans();

private:
    std::vector< Edge* >* activePlan;
};

#endif // ACTIVEPLAN_H
