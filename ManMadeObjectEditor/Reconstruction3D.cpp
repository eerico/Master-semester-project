#include "Reconstruction3D.h"

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec *>* triangles)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles)
{

}

void Reconstruction3D::reconstruct()
{

}

// voir fig. 8, calcul intersection pour l active plan
// et met dans la priority queue. Doit etre
// appeler si edge direction event
void Reconstruction3D::computeIntersection()
{
    foreach(Vertex* vertex, activePlan) {
        foreach(Vertex* vertex2, activePlan) {
            Edge* edge1 = vertex2->getEdge2();
            Edge* edge2 = vertex->getEdge1();
            Edge* edge3 = vertex->getEdge2();

            Intersection intersection = intersect(edge1, edge2, edge3);
            priorityQueue.push(intersection);
        }
    }
}

Reconstruction3D::Intersection Reconstruction3D::intersect(Edge *edge1, Edge *edge2, Edge *edge3)
{
    Intersection intersection;

    return intersection;
}

// faire les truc magique. Pas oublier de update le active plan, et de set les profiles
// (pVertex) a la bonne hauteur
void Reconstruction3D::handleEvent(Intersection intersection)
{

}
