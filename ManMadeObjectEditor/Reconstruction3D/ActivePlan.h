#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H

#include "Geometry/Vertex.h"

class ActivePlan
{
public:
    ActivePlan(Vertex* planVertex, int planSize);

    void computeIntersections();

    bool isIntersectionCorrect();


private:
    Vertex* planVertex;
    int planSize;
};

#endif // ACTIVEPLAN_H
