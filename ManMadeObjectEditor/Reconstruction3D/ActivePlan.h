#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H

#include "../Geometry/Vertex.h"
//#include "Reconstruction3D.h"
#include "Plan.h"
#include "GeneralEvent.h"

class Reconstruction3D;

class ActivePlan
{
public:
    ActivePlan(Vertex* planVertex, int planSize, Reconstruction3D* reconstruction3d);

    void computeIntersections();

    bool isIntersectionCorrect();

    void updateAtCurrentHeight();

private:
    Reconstruction3D* reconstruction3d;
    Vertex* planVertex;
    int planSize;

    bool isIntersectionCorrect(GeneralEvent* intersection, Edge* edge3);

};

#endif // ACTIVEPLAN_H
