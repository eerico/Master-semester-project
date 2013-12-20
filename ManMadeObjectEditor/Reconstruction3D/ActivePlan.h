#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H

#include "Geometry/Vertex.h"

class ActivePlan
{
public:
    ActivePlan();

    void computeIntersections();

    bool isIntersectionCorrect();
};

#endif // ACTIVEPLAN_H
