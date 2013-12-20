#include "ActivePlan.h"
#include "Reconstruction3D.h"

ActivePlan::ActivePlan(Vertex *planVertex, int planSize, Reconstruction3D* reconstruction3d): reconstruction3d(reconstruction3d)
{

    // faire une arraylist de edge pour representer sa.
    // changer comment on detecte les intersection

    //build a copy of the received plan:
    Vertex* iterator = planVertex;
    unsigned int size = planSize;

    Vertex* previousVertex;
    Profile* firstProfile;
    Vertex* firstVertex;

    for(unsigned int i(0); i < size; ++i) {
        Vertex* clone = new Vertex(iterator->getX(), iterator->getY(), iterator->getZ());

        if(i == 0) {
            firstVertex = clone;
            firstProfile = iterator->getEdge1()->getProfile();
        } else {
            clone->setNeighbor1(previousVertex);
            previousVertex->setNeighbor2(clone);

            Profile* profile = iterator->getEdge1()->getProfile();
            Edge* cloneEdge = new Edge(previousVertex, clone, profile);
            activePlan.push_back(cloneEdge);
            Plan* plan = new Plan(cloneEdge->getVertex1(), cloneEdge->getVertex2(), profile);
            plan->computePlanNormal();
            cloneEdge->setDirectionPlan(plan);

            clone->setEdge1(cloneEdge);
            previousVertex->setEdge2(cloneEdge);
        }

        previousVertex = clone;
        iterator = iterator->getNeighbor2();
    }
    firstVertex->setNeighbor1(previousVertex);
    previousVertex->setNeighbor2(firstVertex);

    Edge* cloneEdge = new Edge(previousVertex, firstVertex, firstProfile);
    activePlan.push_back(cloneEdge);
    Plan* plan = new Plan(cloneEdge->getVertex1(), cloneEdge->getVertex2(), firstProfile);
    plan->computePlanNormal();
    cloneEdge->setDirectionPlan(plan);

    firstVertex->setEdge1(cloneEdge);
    previousVertex->setEdge2(cloneEdge);

}

void ActivePlan::computeIntersections()
{
    foreach(Edge* edge1, activePlan) {
        Edge* edge2 = edge1->getVertex2()->getEdge2();
        foreach(Edge* edge3, activePlan) {
            if(edge3 == edge1 || edge3 == edge2) {
                continue;
            }

            Plan * plan1 = edge1->getDirectionPlan();
            Plan * plan2 = edge2->getDirectionPlan();
            Plan * plan3 = edge3->getDirectionPlan();

            //compute intersection:
            GeneralEvent* intersection = plan1->intersect3Plans(plan2, plan3);

            //test if intersection is correct
            if(intersection != 0){
                if(isIntersectionCorrect(intersection, edge3)){
                    reconstruction3d->priorityQueue->push(intersection);
                }
            }
        }
    }
}

bool ActivePlan::isIntersectionCorrect(GeneralEvent* intersection, Edge* edge3)
{
    Plan horizontalPlan (0.f,0.f,intersection->getZ());
    GeneralEvent * intersection1 = horizontalPlan.intersect3Plans(edge3->getDirectionPlan(), edge3->getVertex1()->getEdge1()->getDirectionPlan());
    GeneralEvent * intersection2 = horizontalPlan.intersect3Plans(edge3->getDirectionPlan(), edge3->getVertex2()->getEdge2()->getDirectionPlan());

    if(intersection1 == 0 || intersection2 == 0){
        return false;
    }

    Vertex v1(intersection1->getX(),intersection1->getY(),intersection1->getZ());
    Vertex v2(intersection2->getX(),intersection2->getY(),intersection2->getZ());

    Edge edge(&v1,&v2);

    Vertex intersectionVertex(intersection->getX(),intersection1->getY(), intersection->getZ());
    return (edge.distanceXY(&intersectionVertex) < 0.001);
}

void ActivePlan::updateAtCurrentHeight()
{

}
