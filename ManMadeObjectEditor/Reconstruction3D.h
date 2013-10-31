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
    void reconstruct();
private:
    Vertex* floorPlan;
    unsigned int floorPlanSize;
    std::vector<qglviewer::Vec *>* triangles;
    std::vector<Vertex*> activePlan;
    float currentHeight;

    enum EventType {
        Undefined,
        General,
        EdgeDirection
    };

    struct Intersection {
        Edge* edge1;
        Edge* edge2;
        Edge* edge3;

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

    std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparison> priorityQueue;

    void computeIntersection();
    Intersection intersect(Edge* edge1, Edge* edge2, Edge* edge3, float currentHeight);
    void handleEvent(Intersection& intersection);
    void sphericalToCartesian(Vertex* vertex1, Vertex* vertex2, float& nx, float& ny, float& nz);
};

#endif // RECONSTRUCTION3D_H
