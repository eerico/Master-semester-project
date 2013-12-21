#ifndef RECONSTRUCTION3D_H
#define RECONSTRUCTION3D_H

#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "../Geometry/Profile.h"
#include <vector>
#include <QGLViewer/qglviewer.h>
#include <queue>
#include "../GarbageCollector/GeneralDestructorManager.h"
#include "Event.h"
#include "EventComparator.h"
#include "ActivePlan.h"

class Reconstruction3D
{
public:
    Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector< qglviewer::Vec * >* triangles
                     , std::vector<std::vector<std::vector<Edge *> > > *chainsDebug
                     , std::vector<std::vector<std::vector<Edge *> > > *chainsDebug2
                     , std::vector<std::vector<Edge *> >*activePlanDebug);

    ~Reconstruction3D();
    void reconstruct();

    Vertex* floorPlan;
    unsigned int floorPlanSize;
    std::vector< qglviewer::Vec * >* triangles;
    std::priority_queue<Event*, std::vector<Event*>, EventComparator>* priorityQueue;
    ActivePlan* activePlan;

    static const float deltaHeight;

    // debug information
    std::vector< std::vector< std::vector< Edge* > > >* chainsDebug;
    std::vector< std::vector< std::vector< Edge* > > >* chainsDebug2;
    std::vector< std::vector< Edge* > >* activePlanDebug;


};

#endif // RECONSTRUCTION3D_H
