#include "ActivePlan.h"
#include "Reconstruction3D.h"

ActivePlan::ActivePlan(Vertex *planVertex, int planSize, Reconstruction3D* reconstruction3d): planSize(planSize), reconstruction3d(reconstruction3d)
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
    Vertex * firstVertex = planVertex;
    Vertex * iterator = planVertex->getNeighbor2();

    while (iterator !=firstVertex){
        Vertex*  thirdVertex = iterator->getNeighbor2()->getNeighbor2();

        Plan * plan1 = iterator->getEdge2()->getDirectionPlan();
        Plan * plan2 = iterator->getNeighbor2()->getEdge2()->getDirectionPlan();
        Plan * plan3 = thirdVertex->getEdge2()->getDirectionPlan();

        for(int i(0); i < (planSize -2 -1); i++){//plansize -2 = number of edges left -1 since we don't want to compute multiple times the same intersections

            //compute intersection:
            GeneralEvent* intersection = plan1->intersect3Plans(plan2, plan3);

            //test if intersection is correct
            if(intersection != 0){
                if(isIntersectionCorrect(intersection)){
                    reconstruction3d->priorityQueue->push(intersection);
                }

            }
            //for next intersection:
            thirdVertex = thirdVertex->getNeighbor2();
            plan3 = thirdVertex->getEdge2()->getDirectionPlan();
        }
    }

    // iterator is the first vertex
    Vertex*  thirdVertex = iterator->getNeighbor2()->getNeighbor2();
    Plan * plan1 = iterator->getEdge2()->getDirectionPlan();
    Plan * plan2 = iterator->getNeighbor2()->getEdge2()->getDirectionPlan();
    Plan * plan3 = thirdVertex->getEdge2()->getDirectionPlan();

    for(int i(0); i < (planSize -2); i++){//plansize -2 = number of edges left this time we must compute all intersection hence there is no -1
        //compute intersection:
        GeneralEvent * intersection = plan1->intersect3Plans(plan2, plan3);

        //test if intersection is correct
        if(intersection != 0){
            if(isIntersectionCorrect(intersection)){
                reconstruction3d->priorityQueue->push(intersection);
            }

        }
        //for next intersection:
        thirdVertex = thirdVertex->getNeighbor2();
        plan3 = thirdVertex->getEdge2()->getDirectionPlan();
    }

}

bool ActivePlan::isIntersectionCorrect(GeneralEvent* intersection)
{
    return false;
}
