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
    std::vector< Edge* >* activePlan;

    std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>* priorityQueue;


    void computeIntersection();

    void addEdgeDirectionEvent();

    void removeInvalidIntersection(Edge* edge, float height);

    Intersection intersect(Edge* edge1, Edge* edge2, Edge* edge3);

    void handleEvent(Intersection& intersection);

    void eventClustering(Intersection& intersection);

    void removeDuplicateEdges(Intersection& intersection);

    std::vector< Edge* >* cloneActivePlan();

    void addNewTriangle(Vertex* vertex1, Vertex* vertex2, Vertex* vertex3);

    void removeInvalidEdge(std::vector< Edge* >* edges);

    void edgeDirectionHandling(Intersection& intersection);

    void printActivePlan();

    void pointerCloneActivePlan(std::vector< Edge* >& clone);

    void replaceParentByChild(Intersection& intersection);

    void addChildIntoVector(Edge* parent, std::vector< Edge* >* vector);
};

#endif // RECONSTRUCTION3D_H
