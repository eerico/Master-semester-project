#include "ActivePlan.h"

ActivePlan::ActivePlan(Vertex *planVertex, int planSize): planSize(planSize)
{
    //build a copy of the received plan:
    Vertex* iterator = planVertex;
    unsigned int size = planSize;

    Vertex* previousVertex;
    this->planVertex;

    for(unsigned int i(0); i < size; ++i) {
        Vertex* clone = new Vertex(iterator->getX(), iterator->getY(), iterator->getZ());

        if(i == 0) {
            this->planVertex = clone;
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
    this->planVertex->setNeighbor1(previousVertex);
    previousVertex->setNeighbor2(this->planVertex);
    Edge* cloneEdge = new Edge(previousVertex, this->planVertex);
    this->planVertex->setEdge1(cloneEdge);
    previousVertex->setEdge2(cloneEdge);

}

void ActivePlan::computeIntersections()
{

}

bool ActivePlan::isIntersectionCorrect()
{

}
