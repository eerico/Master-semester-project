#ifndef RECONSTRUCTION3D_H
#define RECONSTRUCTION3D_H

#include "Vertex.h"
#include "Edge.h"
#include "Profile.h"
#include <vector>
#include <QGLViewer/qglviewer.h>
#include <queue>


class Reconstruction3D
{
public:
    Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec *>* triangles);
    ~Reconstruction3D();
    void reconstruct();
private:
    Vertex* floorPlan;
    unsigned int floorPlanSize;
    std::vector<qglviewer::Vec *>* triangles;
    std::vector< Edge* >* activePlan;
    std::vector< std::vector< Edge* >* >* allActivePlan;
    float currentHeight;

    enum EventType {
        NoIntersection,
        General,
        EdgeDirection
    };

    struct Intersection {
        std::vector<Edge*>* edgeVector;

        //intersection point
        float x;
        float y;
        float z;

        EventType eventType;
    };

    class IntersectionComparison
    {
    public:
        bool operator()(Intersection& intersection1, Intersection& intersection2){
            return intersection1.y > intersection2.y;
        }
    };

    std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparison>* priorityQueue;

    // ok mais foreach pourrait etre changer non ?
    void computeIntersection();

    // ok
    void addEdgeDirectionEvent();

    void removeInvalidIntersection(Edge* edge, float height);

    //ok
    Intersection intersect(Edge* edge1, Edge* edge2, Edge* edge3, float currentHeight);

    void handleEvent(Intersection& intersection);

    //ok
    void computePlanNormal(Vertex* vertex1, Vertex* vertex2, Profile* profile, float& nx, float& ny, float& nz);

    //ok
    void eventClustering(Intersection& intersection);

    //ok
    void chainConstruction(Intersection& intersection, std::vector< std::vector< Edge* >* >& chains);

    void intraChainHandling(std::vector< std::vector< Edge* >* >& chains, Intersection& intersection);

    void interChainHandling(std::vector< std::vector< Edge* >* >& chains, Intersection& intersection);

    void splitEdgeAtCorner(Edge* edgeToSplit, Intersection& cornerIntersection, Edge* newEdge1, Edge* newEdge2);

    std::vector< Edge* >* cloneActivePlan();
};

#endif // RECONSTRUCTION3D_H
