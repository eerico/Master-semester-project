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

            Profile* profile = clone->getEdge1()->getProfile();
            Edge* cloneEdge = new Edge(previousVertex, clone, profile);
            Plan* plan = new Plan(cloneEdge->getVertex1(), cloneEdge->getVertex2(), profile);
            cloneEdge->setDirectionPlan(plan);

            clone->setEdge1(cloneEdge);
            previousVertex->setEdge2(cloneEdge);
        }

        previousVertex = clone;
        iterator = iterator->getNeighbor2();
    }
    this->planVertex->setNeighbor1(previousVertex);
    previousVertex->setNeighbor2(this->planVertex);

    Profile* profile = this->planVertex->getEdge1()->getProfile();
    Edge* cloneEdge = new Edge(previousVertex, this->planVertex);
    Plan* plan = new Plan(cloneEdge->getVertex1(), cloneEdge->getVertex2(), profile);
    cloneEdge->setDirectionPlan(plan);

    this->planVertex->setEdge1(cloneEdge);
    previousVertex->setEdge2(cloneEdge);

}

void ActivePlan::computeIntersections()
{

}

bool ActivePlan::isIntersectionCorrect()
{
    return false;
}
