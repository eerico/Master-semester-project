#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H

#include "../Geometry/Vertex.h"
#include "Reconstruction3D.h"
#include "Plan.h"
#include "GeneralEvent.h"

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

    bool isIntersectionCorrect(GeneralEvent * intersection);

};

#endif // ACTIVEPLAN_H
