#include "ActivePlan.h"

ActivePlan::ActivePlan(Vertex *floorPlan, unsigned int floorPlanSize)
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
}

std::vector< Edge* >* ActivePlan::getPlans() {
    return activePlan;
}
