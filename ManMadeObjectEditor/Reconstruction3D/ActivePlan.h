#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H
#include <vector>
#include <QGLViewer/qglviewer.h>
#include "../Geometry/Edge.h"
#include "../Geometry/Vertex.h"
#include "../Geometry/Profile.h"
#include "Intersection.h"
#include "Plan.h"

class ActivePlan
{
public:
    ActivePlan(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles);
    ActivePlan(ActivePlan* previousActivePlan, std::vector<qglviewer::Vec *> *triangles, bool &chainSizeOne);
    std::vector< Edge* >* getPlan();
    void computeDirectionPlan();
    bool filteringInvalidEvent(Intersection& intersection);
    void filteringInvalidEvent2(Intersection& intersection);
    void insert2Edges(Edge* oldEdge, Edge* newEdge1, Edge* newEdge2);
    unsigned int numberValidEdge();
    void print(bool onlyValidEdge);

    void removeInvalidEdges();

    void fillHoles();

    void getActivePlanCopy(std::vector<std::vector<Edge *> > *copy);

private:
    std::vector< Edge* >* activePlan;
    std::vector<qglviewer::Vec * > *triangles;
    void addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3);
    std::vector< std::vector< Edge* >* >* createChain(ActivePlan* previousActivePlan);
};

#endif // ACTIVEPLAN_H
