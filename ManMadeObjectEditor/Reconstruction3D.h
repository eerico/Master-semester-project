#ifndef RECONSTRUCTION3D_H
#define RECONSTRUCTION3D_H

#include "Geometry/Vertex.h"
#include "Geometry/Edge.h"
#include "Geometry/Profile.h"
#include <vector>
#include <QGLViewer/qglviewer.h>
#include <queue>


class Reconstruction3D
{
public:
    Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * const>* triangles);
    ~Reconstruction3D();
    void reconstruct();
private:
    Vertex* floorPlan;
    unsigned int floorPlanSize;
    std::vector<qglviewer::Vec *const >* triangles;
    std::vector< Edge* >* activePlan;
    std::vector< std::vector< Edge* >* >* allActivePlan;

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
            return intersection1.z > intersection2.z;
        }
    };

    struct Plan {
        // a plan is defined by a point and a normal
        float pointX;
        float pointY;
        float pointZ;

        float normalX;
        float normalY;
        float normalZ;
    };

    std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparison>* priorityQueue;

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
    // appel commenter dans le code
    void interChainHandling(std::vector< std::vector< Edge* >* >& chains, Intersection& intersection);

    // marche pas correctement cas simple
    void splitEdgeAtCorner(Edge* edgeToSplit, Intersection& cornerIntersection, Edge*& newEdge1, Edge*& newEdge2);

    // ok
    std::vector< Edge* >* cloneActivePlan();

    //ok
    void addNewTriangle(Vertex* vertex1, Vertex* vertex2, Vertex* vertex3);

    void removeInvalidEdge(std::vector< Edge* >* edges);

    Intersection intersect3Plans(Plan& plan1, Plan& plan2, Plan& plan3);

    void edgeDirectionHandling(Intersection& intersection);

    //bool isIntersectionExternToEdges(Intersection& intersection);
};

#endif // RECONSTRUCTION3D_H
