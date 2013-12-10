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

    if(maxDistance > threshold) {

        Curve* leftCurve = new Curve;
        leftCurve->begin = curve->begin;
        leftCurve->end = farthestVertex;
        leftCurve->size = index + 1;

        Curve* rightCurve = new Curve;
        rightCurve->begin = farthestVertex;
        rightCurve->end = curve->end;
        rightCurve->size = curve->size - index;

        simplifyFloorPlan(leftCurve);
        simplifyFloorPlan(rightCurve);

        delete leftCurve;
        delete rightCurve;
    } else {
        Profile* oldProfile = curve->begin->getEdge2()->getProfile();
        delete curve->begin->getEdge2();
        Vertex* iterator = curve->begin->getNeighbor2();
        for(int i(0); i < curve->size - 2; ++i) {
            Vertex* toDelete = iterator;
            iterator = iterator->getNeighbor2();

            delete toDelete->getEdge2();
            delete toDelete;

            meshManager->decrementFloorPlanSize();
        }

        Vertex* vertex1 = curve->begin;
        Vertex* vertex2 = curve->end;
        Edge* edge = new Edge(vertex1, vertex2, oldProfile);
        vertex1->setEdge2(edge);
        vertex2->setEdge1(edge);
        vertex1->setNeighbor2(vertex2);
        vertex2->setNeighbor1(vertex1);

        meshManager->setFloorPlan(curve->begin);
        meshManager->setCurrentProfile(oldProfile);
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

    if(maxDistance > threshold) {
        Curve* leftCurve = new Curve;
        leftCurve->begin = curve->begin;
        leftCurve->end = farthestVertex;
        leftCurve->size = index + 1;

        Curve* rightCurve = new Curve;
        rightCurve->begin = farthestVertex;
        rightCurve->end = curve->end;
        rightCurve->size = curve->size - index;

        simplifyProfile(leftCurve);
        simplifyProfile(rightCurve);

        delete leftCurve;
        delete rightCurve;
    } else {
        delete curve->begin->getEdge2();
        Vertex* iterator = curve->begin->getNeighbor2();
        for(int i(0); i < curve->size - 2; ++i) {
            Vertex* toDelete = iterator;
            iterator = iterator->getNeighbor2();

            delete toDelete->getEdge2();
            delete toDelete;
        }

        Vertex* vertex1 = curve->begin;
        Vertex* vertex2 = curve->end;
        Edge* edge = new Edge(vertex1, vertex2);
        vertex1->setEdge2(edge);
        vertex2->setEdge1(edge);
        vertex1->setNeighbor2(vertex2);
        vertex2->setNeighbor1(vertex1);
        edge->computeLineItem();
    }
}

