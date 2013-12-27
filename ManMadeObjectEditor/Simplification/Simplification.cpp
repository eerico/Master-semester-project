#include "Simplification.h"

// Simplification algorithm based on the Ramer–Douglas–Peucker algorithm

Simplification::Simplification(std::vector<Curve *> *curveArray, MeshManager* meshManager, float threshold)
    :curveArray(curveArray), threshold(threshold), meshManager(meshManager)
{

}

void Simplification::simplifyFloorPlan() {
    // apply the simplification algorithm for every curve
    foreach(Curve* curve, *curveArray) {
        if (curve->size > 2) {
            simplifyFloorPlan(curve, false);
        }
    }

    meshManager->emitNewFloorPlan();
    meshManager->emitNewProfileSelected();
}

void Simplification::simplifyFloorPlan(Curve* curve, bool forPreview) {
    // if the curve is less than 3 vertices, there is nothing to simplify and
    // we want the floor plan to have at least 3 vertices
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

    // if the distant of the farthest vertex is above a threshold then this
    // vertex must not be deleted and the simplification algorithm is recursively done
    // at the left and right side of this vertex
    if(maxDistance > threshold) {

        Curve* leftCurve = new Curve;
        leftCurve->begin = curve->begin;
        leftCurve->end = farthestVertex;
        leftCurve->size = index + 1;

        Curve* rightCurve = new Curve;
        rightCurve->begin = farthestVertex;
        rightCurve->end = curve->end;
        rightCurve->size = curve->size - index;

        simplifyFloorPlan(leftCurve, forPreview);
        simplifyFloorPlan(rightCurve, forPreview);

        delete leftCurve;
        delete rightCurve;
    } else {
        // the more distant vertex in the curve is below the threshold.
        // We will thus delete all vertex defining by the current curve (except the first and last vertex)

        Profile* oldProfile = curve->begin->getEdge2()->getProfile();
        delete curve->begin->getEdge2();
        Vertex* iterator = curve->begin->getNeighbor2();
        for(int i(0); i < curve->size - 2; ++i) {
            Vertex* toDelete = iterator;
            iterator = iterator->getNeighbor2();

            delete toDelete->getEdge2();
            delete toDelete;

            if(!forPreview) {
                meshManager->decrementFloorPlanSize();
            }
        }

        Vertex* vertex1 = curve->begin;
        Vertex* vertex2 = curve->end;
        Edge* edge = new Edge(vertex1, vertex2, oldProfile);
        vertex1->setEdge2(edge);
        vertex2->setEdge1(edge);
        vertex1->setNeighbor2(vertex2);
        vertex2->setNeighbor1(vertex1);

        if(!forPreview) {
            meshManager->setFloorPlan(curve->begin);
            meshManager->setCurrentProfile(oldProfile);
        }
    }

    if(!forPreview) {
        meshManager->setEdgeSelected(0);
    }
}

void Simplification::simplifyProfile() {
    // apply the simplification algorithm for every curve
    foreach(Curve* curve, *curveArray) {
        if (curve->size > 2) {
            simplifyProfile(curve, false);
        }
    }
    meshManager->emitDrawWithoutDeleteOldProfile();
    meshManager->setUpdateOnMesh();
}

void Simplification::simplifyProfile(Curve* curve, bool forPreview) {
    // if the curve is less than 3 vertices, there is nothing to simplify
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

    // if the distant of the farthest vertex is above a threshold then this
    // vertex must not be deleted and the simplification algorithm is recursively done
    // at the left and right side of this vertex
    if(maxDistance > threshold) {
        Curve* leftCurve = new Curve;
        leftCurve->begin = curve->begin;
        leftCurve->end = farthestVertex;
        leftCurve->size = index + 1;

        Curve* rightCurve = new Curve;
        rightCurve->begin = farthestVertex;
        rightCurve->end = curve->end;
        rightCurve->size = curve->size - index;

        simplifyProfile(leftCurve, forPreview);
        simplifyProfile(rightCurve, forPreview);

        delete leftCurve;
        delete rightCurve;
    } else {
        // the more distant vertex in the curve is below the threshold.
        // We will thus delete all vertex defining by the current curve (except the first and last vertex)

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

        if(!forPreview) {
            edge->computeLineItem();
        }
    }
}

std::vector<Vertex*>* Simplification::simplifyFloorPlanPreview() {

    //first clone the curveArray
    std::vector<Curve*> curveArrayTmp;
    foreach(Curve* curve, *curveArray) {
        Curve* clone = new Curve;
        clone->begin = curve->begin;
        clone->end = curve->end;
        clone->size = curve->size;
        curveArrayTmp.push_back(clone);
    }

    // then clone the floor plan
    // and update the vertex stored in the curveArray

    Vertex* iterator = meshManager->getFloorPlan();
    unsigned int size = meshManager->getFloorPlanSize();

    Vertex* previousVertex;
    Vertex* firstVertex;

    for(unsigned int i(0); i < size; ++i) {
        Vertex* clone = new Vertex(iterator->getX(), iterator->getY(), iterator->getZ());

        updateCurveArray(&curveArrayTmp, iterator, clone);

        if(i == 0) {
            firstVertex = clone;
        } else {
            clone->setNeighbor1(previousVertex);
            previousVertex->setNeighbor2(clone);

            Edge* cloneEdge = new Edge(previousVertex, clone);
            clone->setEdge1(cloneEdge);
            previousVertex->setEdge2(cloneEdge);
        }

        previousVertex = clone;
        iterator = iterator->getNeighbor2();
    }
    firstVertex->setNeighbor1(previousVertex);
    previousVertex->setNeighbor2(firstVertex);
    Edge* cloneEdge = new Edge(previousVertex, firstVertex);
    firstVertex->setEdge1(cloneEdge);
    previousVertex->setEdge2(cloneEdge);

    //now do the simplification on the copy
    foreach(Curve* curve, curveArrayTmp) {
        if (curve->size > 2) {
            simplifyFloorPlan(curve, true);
        }
    }

    Curve* curveTmp = curveArrayTmp[0];
    iterator = curveTmp->begin;

    //clean the data
    foreach(Curve* curve, curveArrayTmp) {
        delete curve;
    }

    //contruct the returned value
    std::vector<Vertex*>* preview = new std::vector<Vertex*>;

    firstVertex = iterator;
    do{
        preview->push_back(iterator);
        iterator = iterator->getNeighbor2();
    }while(iterator != firstVertex);

    return preview;
}

std::vector<Vertex*>* Simplification::simplifyProfilePreview() {

    //first clone the curveArray
    std::vector<Curve*> curveArrayTmp;
    foreach(Curve* curve, *curveArray) {
        Curve* clone = new Curve;
        clone->begin = curve->begin;
        clone->end = curve->end;
        clone->size = curve->size;
        curveArrayTmp.push_back(clone);
    }

    // then clone the profile
    // and update the vertex stored in the curveArray

    Profile* profile = meshManager->getCurrentProfile();
    Vertex* iterator = profile->getProfileVertex();

    Vertex* previousVertex = new Vertex(iterator->getX(), iterator->getY(), iterator->getZ());
    updateCurveArray(&curveArrayTmp, iterator, previousVertex);
    previousVertex->setEdge1(0);
    previousVertex->setNeighbor1(0);

    iterator = iterator->getNeighbor2();
    while(iterator != 0) {
        Vertex* clone = new Vertex(iterator->getX(), iterator->getY(), iterator->getZ());

        updateCurveArray(&curveArrayTmp, iterator, clone);

        clone->setNeighbor1(previousVertex);
        previousVertex->setNeighbor2(clone);

        Edge* cloneEdge = new Edge(previousVertex, clone);
        clone->setEdge1(cloneEdge);
        previousVertex->setEdge2(cloneEdge);


        previousVertex = clone;
        iterator = iterator->getNeighbor2();
    }
    previousVertex->setEdge2(0);
    previousVertex->setNeighbor2(0);

    //now do the simplification on the copy
    foreach(Curve* curve, curveArrayTmp) {
        if (curve->size > 2) {
            simplifyProfile(curve, true);
        }
    }

    Curve* curveTmp = curveArrayTmp[0];
    iterator = curveTmp->begin;

    //clean the data
    foreach(Curve* curve, curveArrayTmp) {
        delete curve;
    }

    //contruct the returned value
    std::vector<Vertex*>* preview = new std::vector<Vertex*>;

    Vertex* firstVertex = iterator;
    while(iterator != 0){
        preview->push_back(iterator);
        iterator = iterator->getNeighbor2();
    }

    iterator = firstVertex->getNeighbor1();
    while(iterator != 0){
        preview->push_back(iterator);
        iterator = iterator->getNeighbor1();
    }

    return preview;
}

void Simplification::updateCurveArray(std::vector<Curve *> *curveArrayUpdated, Vertex* original, Vertex* clone) {
    unsigned int size = curveArrayUpdated->size();
    for(unsigned int i(0); i < size; ++i) {
        Curve* currentCurve = (*curveArrayUpdated)[i];
        if(currentCurve->begin == original) {
            currentCurve->begin = clone;
        }
        if(currentCurve->end == original) {
            currentCurve->end = clone;
        }
    }
}
