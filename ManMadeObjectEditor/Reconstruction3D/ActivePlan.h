#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H

#include "../Geometry/Vertex.h"
#include "Reconstruction3D.h"
#include "Plan.h"

class ActivePlan
{
public:
    ActivePlan(Vertex* planVertex, int planSize, Reconstruction3D* reconstruction3d);

    void computeIntersections();

    bool isIntersectionCorrect();


private:
    Reconstruction3D* reconstruction3d;
    Vertex* planVertex;
    int planSize;
};

#endif // ACTIVEPLAN_H
