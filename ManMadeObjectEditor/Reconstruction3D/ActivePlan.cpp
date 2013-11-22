#include "ActivePlan.h"

#define DEBUG

ActivePlan::ActivePlan(Vertex *floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec *> *triangles)
    : triangles(triangles)
{
    Vertex* currentVertex = floorPlan;
    activePlan = new std::vector< Edge* >;

    Vertex* firstCloneVertex;
    Vertex* previousVertex;
    Edge* previousEdge;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        Edge* currentEdge = currentVertex->getEdge2();

        Edge* cloneEdge = new Edge(currentEdge->getVertex1(), currentEdge->getVertex2(), currentEdge->getProfile());
        Vertex* cloneVertex1 = new Vertex(currentVertex->getX(), currentVertex->getY(), currentVertex->getZ());

        cloneVertex1->setEdge2(cloneEdge);
        cloneEdge->setVertex1(cloneVertex1);

        if(i > 0) {
            cloneVertex1->setEdge1(previousEdge);
            cloneVertex1->setNeighbor1(previousVertex);
            previousVertex->setNeighbor2(cloneVertex1);
            previousEdge->setVertex2(cloneVertex1);
        } else {
            firstCloneVertex = cloneVertex1;
        }

        previousVertex = cloneVertex1;
        previousEdge = cloneEdge;

        activePlan->push_back(cloneEdge);
        currentVertex = currentVertex->getNeighbor2();
    }

    firstCloneVertex->setEdge1(previousEdge);
    firstCloneVertex->setNeighbor1(previousVertex);
    previousVertex->setNeighbor2(firstCloneVertex);
    previousEdge->setVertex2(firstCloneVertex);


    // check if two edge are parallel
    // if there are, we remove the connection between them
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        Edge* currentEdge = (*activePlan)[i];
        Edge* nextEdge = (*activePlan)[(i+1) % floorPlanSize];

        if (currentEdge->isParallel(nextEdge)) {
            //Vertex* vertexToRemove = currentEdge->getVertex2();
            Vertex* vertex1 = currentEdge->getVertex1();
            Vertex* vertex2 = nextEdge->getVertex2();

            vertex1->setNeighbor2(vertex2);
            vertex2->setNeighbor1(vertex1);

            nextEdge->setVertex1(vertex1);
            vertex1->setEdge2(nextEdge);

            activePlan->erase(activePlan->begin() + i);
            floorPlanSize--;
            i--;
        }
    }






#ifdef DEBUG
    foreach(Edge* e, *activePlan) {
        Vertex* v1 = e->getVertex1();
        Vertex* v2 = e->getVertex2();
        Vertex* vv = new Vertex((v1->getX() + v2->getX()) / 2.0f + 0.002f, (v1->getY() + v2->getY()) / 2.0f + + 0.002f, v1->getZ());
        addNewTriangle(v1, v2, vv);
    }
#endif
}

// Constructor only used for debugging
ActivePlan::ActivePlan(float height, ActivePlan* previousActivePlan, std::vector< qglviewer::Vec * >* triangles)
    :previousActivePlan(previousActivePlan), triangles(triangles)
{
    activePlan = new std::vector< Edge* >;

    Plan horizontalPlan(0.0f, 0.0f, height, 0.0f, 0.0f, 1.0f);

    std::vector< Edge* >* edges = previousActivePlan->getPlan();
    unsigned int numberEdge = edges->size();

    Vertex* firstNewVertex;
    Vertex* previousNewVertex;
    Profile* previousProfile;
    Vertex* newVertex;
    Edge* newEdge;
    Edge* firstPreviousEdge;

    for(unsigned int edgeIndex(0); edgeIndex < numberEdge; ++edgeIndex) {

        Edge* currentEdge = (*edges)[edgeIndex];

        Vertex* vertex1 = currentEdge->getVertex1();
        Vertex* vertex2 = currentEdge->getVertex2();

        Edge* previousEdge = vertex1->getEdge1();
        Vertex* vertex0 = previousEdge->getVertex1();

        Plan plan1(vertex0->getX(), vertex0->getY(), vertex0->getZ());
        plan1.computePlanNormal(vertex0, vertex1, previousEdge->getProfile());

        Plan plan2(vertex1->getX(), vertex1->getY(), vertex1->getZ());
        plan2.computePlanNormal(vertex1, vertex2, currentEdge->getProfile());

        Intersection newIntersection = horizontalPlan.intersect3Plans(&plan1, &plan2);

        newVertex = new Vertex(newIntersection.x, newIntersection.y, newIntersection.z);

        if (edgeIndex > 0) {
            newEdge = new Edge(previousNewVertex, newVertex, previousProfile);

            previousEdge->setChild1(newEdge);
            newEdge->setParent(previousEdge);

            previousNewVertex->setEdge2(newEdge);
            previousNewVertex->setNeighbor2(newVertex);
            newVertex->setEdge1(newEdge);
            newVertex->setNeighbor1(previousNewVertex);


            activePlan->push_back(newEdge);
        } else {
            firstNewVertex = newVertex;
            firstPreviousEdge = previousEdge;
        }

        previousNewVertex = newVertex;
        previousProfile = currentEdge->getProfile();
    }

    newEdge = new Edge(previousNewVertex, firstNewVertex, previousProfile);

    firstPreviousEdge->setChild1(newEdge);
    newEdge->setParent(firstPreviousEdge);

    previousNewVertex->setEdge2(newEdge);
    previousNewVertex->setNeighbor2(firstNewVertex);
    firstNewVertex->setEdge1(newEdge);
    firstNewVertex->setNeighbor1(previousNewVertex);
    activePlan->push_back(newEdge);

#ifdef DEBUG
    foreach(Edge* e, *activePlan) {
        Vertex* v1 = e->getVertex1();
        Vertex* v2 = e->getVertex2();
        Vertex* vv = new Vertex((v1->getX() + v2->getX()) / 2.0f + 0.002f, (v1->getY() + v2->getY()) / 2.0f + + 0.002f, v1->getZ());
        addNewTriangle(v1, v2, vv);
    }
#endif

}

void ActivePlan::computeDirectionPlan() {
    foreach(Edge* edge, *activePlan) {
        Vertex* vertex1 = edge->getVertex1();
        Plan* plan = new Plan( vertex1->getX(), vertex1->getY(), vertex1->getZ());
        plan->computePlanNormal(vertex1, edge->getVertex2(), edge->getProfile());
        edge->setDirectionPlan(plan);
    }
}

std::vector< Edge* >* ActivePlan::getPlan() {
    return activePlan;
}

/// TODO
// faire une methode qui prend les 3 edge et une intersection et qui test si l'lintersection a bien
// lieu entre les 3 edge à cette hauteur
// Donc dans intersection, faire pour que je stock les 2 neighbor et l edge different afin de faciliter le calcul

void ActivePlan::addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) {
    qglviewer::Vec* triangleVertex1 = new qglviewer::Vec(vertex1->getX(), vertex1->getY(), vertex1->getZ());
    qglviewer::Vec* triangleVertex2 = new qglviewer::Vec(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    qglviewer::Vec* triangleVertex3 = new qglviewer::Vec(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    triangles->push_back(triangleVertex1);
    triangles->push_back(triangleVertex2);
    triangles->push_back(triangleVertex3);
}

