#ifndef RECONSTRUCTION3D_H
#define RECONSTRUCTION3D_H

#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "../Geometry/Profile.h"
#include "Plan.h"
#include "Intersection.h"
#include "IntersectionComparator.h"
#include <vector>
#include <QGLViewer/qglviewer.h>
#include <queue>


class Reconstruction3D
{
public:
    Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec *> *triangles);
    ~Reconstruction3D();
    void reconstruct();
private:
    Vertex* floorPlan;
    unsigned int floorPlanSize;
    std::vector<qglviewer::Vec *>* triangles;
    std::vector< Edge* >* activePlan;
    std::vector< std::vector< Edge* >* >* allActivePlan;

    std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>* priorityQueue;

    // ok
    void computeIntersection();

    // ok
    void addEdgeDirectionEvent();

    // ok
    void removeInvalidIntersection(Edge* edge, float height);

    //ok
    Intersection intersect(Edge* edge1, Edge* edge2, Edge* edge3);

    void handleEvent(Intersection& intersection);

    //ok
    void computePlanNormal(Vertex* vertex1, Vertex* vertex2, Profile* profile, float& nx, float& ny, float& nz);

    //ok
    void eventClustering(Intersection& intersection);

    //ok
    void chainConstruction(Intersection& intersection, std::vector< std::vector< Edge* >* >& chains);

    void removeDuplicateEdges(Intersection& intersection);

    //ok
    void intraChainHandling(std::vector< std::vector< Edge* >* >& chains, Intersection& intersection);

    // marche pas correctement cas simple
    void interChainHandling(std::vector< std::vector< Edge* >* >& chains, Intersection& intersection);

    // marche pas correctement cas simple
    void splitEdgeAtCorner(Edge* edgeToSplit, Intersection& cornerIntersection, Edge*& newEdge1, Edge*& newEdge2);

    // ok
    std::vector< Edge* >* cloneActivePlan();

    //ok
    void addNewTriangle(Vertex* vertex1, Vertex* vertex2, Vertex* vertex3);

    void removeInvalidEdge(std::vector< Edge* >* edges);



    void edgeDirectionHandling(Intersection& intersection);

    //bool isIntersectionExternToEdges(Intersection& intersection);
};

#endif // RECONSTRUCTION3D_H
