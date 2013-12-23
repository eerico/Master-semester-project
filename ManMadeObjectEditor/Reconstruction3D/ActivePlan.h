#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H

#include "../Geometry/Vertex.h"
//#include "Reconstruction3D.h"
#include "Plan.h"
#include "GeneralEvent.h"
#include "EdgeEvent.h"
#include <vector>

class Reconstruction3D;

class ActivePlan
{
public:
    ActivePlan(Vertex* planVertex, int planSize, Reconstruction3D* reconstruction3d);

    void computeIntersections();
    void addEdgeDirectionEvent();

    void updateAtCurrentHeight(float currentHeight);

    void insert2Edges(Edge* old, Edge* new1, Edge* new2);
    void getActivePlanCopy(std::vector< std::vector< Edge* > >* copy);
    void removeInvalidEdges();

    void eliminateParallelNeighbor();

    float size();

    void checkConsistency();

private:
    Reconstruction3D* reconstruction3d;
    std::vector<Edge*> activePlan;

    bool isIntersectionCorrect(GeneralEvent* intersection, Edge* edge3);
    Edge* isIntersectionWithChildCorrect(GeneralEvent* intersection, Edge* old, Edge* child1, Edge* child2);

    void addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3);
};

#endif // ACTIVEPLAN_H
