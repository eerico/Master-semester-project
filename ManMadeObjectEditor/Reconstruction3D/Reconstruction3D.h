#ifndef RECONSTRUCTION3D_H
#define RECONSTRUCTION3D_H

#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "../Geometry/Profile.h"
#include "Plan.h"
#include "Intersection.h"
#include "IntersectionComparator.h"
#include "Chain.h"
#include <vector>
#include <QGLViewer/qglviewer.h>
#include <queue>


class Reconstruction3D
{
public:
    Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector< qglviewer::Vec * >* triangles);
    ~Reconstruction3D();
    void reconstruct();
private:
    Vertex* floorPlan;
    unsigned int floorPlanSize;
    std::vector< qglviewer::Vec * >* triangles;
    Chain* currentChain;

    std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>* priorityQueue;


    void computeIntersection();

    void addEdgeDirectionEvent();


    Intersection intersect(Edge* edge1, Edge* edge2, Edge* edge3);

    void handleEvent(Intersection& intersection);

    void eventClustering(Intersection& intersection);

    void edgeDirectionHandling(Intersection& intersection);

    void printActiveChain();

};

#endif // RECONSTRUCTION3D_H