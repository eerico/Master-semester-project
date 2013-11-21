#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H
#include <vector>
#include <QGLViewer/qglviewer.h>
#include "../Geometry/Edge.h"
#include "../Geometry/Vertex.h"
#include "../Geometry/Profile.h"
#include "Plan.h"

class ActivePlan
{
public:
    ActivePlan(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles);
    ActivePlan(float height, ActivePlan* previousActivePlan, std::vector< qglviewer::Vec * >* triangles);
    std::vector< Edge* >* getPlans();

private:
    std::vector< Edge* >* activePlan;
    std::vector<qglviewer::Vec * > *triangles;
    ActivePlan* previousActivePlan;
    void addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) ;
};

#endif // ACTIVEPLAN_H
