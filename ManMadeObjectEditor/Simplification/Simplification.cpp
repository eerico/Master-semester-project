#include "Simplification.h"

// Simplification algorithm based on the Ramer–Douglas–Peucker algorithm

Simplification::Simplification(std::vector<Curve *> *curveArray, MeshManager* meshManager, float threshold)
    :curveArray(curveArray), threshold(threshold), meshManager(meshManager)
{

}

void Simplification::simplifyFloorPlan() {
    foreach(Curve* curve, *curveArray) {
        if (curve->size > 2) {
            simplifyFloorPlan(curve);
        }
    }

    meshManager->emitNewFloorPlan();
    meshManager->emitNewProfileSelected();
}

void Simplification::simplifyFloorPlan(Curve* curve) {
    if (curve->size < 3 || meshManager->getFloorPlanSize() == 3) {
        return;
    }

    // find the vertex that is the most distant
    // from the line defined by the begining and the end
    // of the curve
    Edge edge(curve->begin, curve->end);

    float maxDistance = FLT_MIN;
    Vertex* farthestVertex(0);
    float index(0);

    Vertex* iterator = curve->begin->getNeighbor2();
    unsigned int size = curve->size - 2;
    for(unsigned int i(0); i < size; ++i) {
        float currentDistance = edge.lineDistance(iterator);
        if(currentDistance > maxDistance) {
            maxDistance = currentDistance;
            farthestVertex = iterator;
            index = i + 1;
        }
        iterator = iterator->getNeighbor2();
    }

    if((maxDistance > threshold) && (farthestVertex != 0)) {
        Edge* previousEdge = farthestVertex->getEdge1();
        Edge* nextEdge = farthestVertex->getEdge2();
        Vertex* vertex1 = farthestVertex->getNeighbor1();
        Vertex* vertex2 = farthestVertex->getNeighbor2();

        Edge* newEdge = new Edge(vertex1, vertex2, previousEdge->getProfile());
        vertex1->setNeighbor2(vertex2);
        vertex2->setNeighbor1(vertex1);
        vertex1->setEdge2(newEdge);
        vertex2->setEdge1(newEdge);

        delete previousEdge;
        delete nextEdge;
        delete farthestVertex;

        meshManager->decrementFloorPlanSize();
        meshManager->setFloorPlan(vertex2);
        meshManager->setCurrentProfile(nextEdge->getProfile());

        Curve* leftCurve = new Curve;
        leftCurve->begin = curve->begin;
        leftCurve->end = vertex1;
        leftCurve->size = index;

        Curve* rightCurve = new Curve;
        rightCurve->begin = vertex2;
        rightCurve->end = curve->end;
        rightCurve->size = curve->size - 1 - index;

        simplifyFloorPlan(leftCurve);
        simplifyFloorPlan(rightCurve);

        delete leftCurve;
        delete rightCurve;
    }

    meshManager->setEdgeSelected(0);
}

void Simplification::simplifyProfile() {
    foreach(Curve* curve, *curveArray) {
        if (curve->size > 2) {
            simplifyProfile(curve);
        }
    }
    meshManager->emitDrawWithoutDeleteOldProfile();
    meshManager->setUpdateOnMesh();
}

void Simplification::simplifyProfile(Curve* curve) {
    if (curve->size < 3) {
        return;
    }

    // find the vertex that is the most distant
    // from the line defined by the begining and the end
    // of the curve
    Edge edge(curve->begin, curve->end);

    float maxDistance = FLT_MIN;
    Vertex* farthestVertex(0);
    float index(0);

    Vertex* iterator = curve->begin->getNeighbor2();
    unsigned int size = curve->size - 2;
    for(unsigned int i(0); i < size; ++i) {
        float currentDistance = edge.lineDistance(iterator);
        if(currentDistance > maxDistance) {
            maxDistance = currentDistance;
            farthestVertex = iterator;
            index = i + 1;
        }
        iterator = iterator->getNeighbor2();
    }

    if((maxDistance > threshold) && (farthestVertex != 0)) {
        Edge* previousEdge = farthestVertex->getEdge1();
        Edge* nextEdge = farthestVertex->getEdge2();
        Vertex* vertex1 = farthestVertex->getNeighbor1();
        Vertex* vertex2 = farthestVertex->getNeighbor2();

        Edge* newEdge = new Edge(vertex1, vertex2);
        vertex1->setNeighbor2(vertex2);
        vertex2->setNeighbor1(vertex1);
        vertex1->setEdge2(newEdge);
        vertex2->setEdge1(newEdge);

        newEdge->computeLineItem();

        delete previousEdge;
        delete nextEdge;
        delete farthestVertex;

        Curve* leftCurve = new Curve;
        leftCurve->begin = curve->begin;
        leftCurve->end = vertex1;
        leftCurve->size = index;

        Curve* rightCurve = new Curve;
        rightCurve->begin = vertex2;
        rightCurve->end = curve->end;
        rightCurve->size = curve->size - 1 - index;

        simplifyProfile(leftCurve);
        simplifyProfile(rightCurve);

        delete leftCurve;
        delete rightCurve;
    }
}

