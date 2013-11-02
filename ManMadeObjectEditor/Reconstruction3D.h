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
    std::vector< Vertex* >* activePlan;
    std::vector< std::vector< Vertex* >* >* allActivePlan;
    float currentHeight;

    enum EventType {
        Undefined,
        General,
        EdgeDirection
    };

    struct Intersection {
        std::vector<Edge*> edgeVector;

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
            return intersection1.y < intersection2.y;
        }
    };

    std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparison>* priorityQueue;

    void computeIntersection();
    void addEdgeDirectionEvent();
    void removeInvalidIntersection(Edge* edge, float height);
    Intersection intersect(Edge* edge1, Edge* edge2, Edge* edge3, float currentHeight);
    void handleEvent(Intersection& intersection);
    void computePlanNormal(Vertex* vertex1, Vertex* vertex2, Profile* profile, float& nx, float& ny, float& nz);
    void eventClustering(Intersection& intersection);
    void chainConstruction(Intersection& intersection, std::vector< std::vector< Edge* > >& chains);
    void intraChainHandling(std::vector< std::vector< Edge* > >& chains);
    void interChainHandling(std::vector< std::vector< Edge* > >& chains);
    std::vector< Vertex* >* clonePlan(std::vector< Vertex* >* activePlan);
};

#endif // RECONSTRUCTION3D_H
