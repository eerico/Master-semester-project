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
        if(edge1->isValid()) {
            Edge* edge2 = edge1->getVertex2()->getEdge2();
            if(edge2->isValid()) {//reste bloque ici mais bon avec error devrait pas etre possible mais... edges child must intersect eh

                foreach(Edge* edge3, activePlan) {
                    if(edge3->isValid()) {
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
                                intersection->addEdge(edge1);
                                intersection->addEdge(edge2);
                                intersection->addEdge(edge3);
                                reconstruction3d->priorityQueue->push(intersection);
                            }
                        }
                    }
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

    Vertex intersectionVertex(intersection->getX(),intersection->getY(), intersection->getZ());
    return (edge.distanceXY(&intersectionVertex) < 0.001);
}

Edge *ActivePlan::isIntersectionWithChildCorrect(GeneralEvent *intersection, Edge *old, Edge *child1, Edge *child2)
{
    Plan horizontalPlan (0.f,0.f,intersection->getZ());
    GeneralEvent * intersection1 = horizontalPlan.intersect3Plans(old->getDirectionPlan(), old->getVertex1()->getEdge1()->getDirectionPlan());
    GeneralEvent * intersection2 = horizontalPlan.intersect3Plans(old->getDirectionPlan(), old->getVertex2()->getEdge2()->getDirectionPlan());

    if(intersection1 == 0 || intersection2 == 0){
        return 0;
    }
    float oldLength = old->getVertex1()->distance(old->getVertex2());
    float child1Length =  child1->getVertex1()->distance(child1->getVertex2());
    float ratio = child1Length/oldLength;

    float x = intersection2->getX() - intersection1->getX() ;
    x= x*ratio;
    float y = intersection2->getY() - intersection1->getY();
    y = y* ratio;

    Vertex newVertex(intersection1->getX()+x,intersection1->getY()+y,intersection1->getZ());

    Vertex v1(intersection1->getX(),intersection1->getY(),intersection1->getZ());
    Vertex v2(intersection2->getX(),intersection2->getY(),intersection2->getZ());

    Vertex intersectionVertex(intersection->getX(),intersection->getY(), intersection->getZ());


    Edge edge(&v1,&newVertex);
    if(edge.distanceXY(&intersectionVertex) < 0.001){
        return child1;
    }else{
        edge= Edge(&newVertex, &v2);
        if(edge.distanceXY(&intersectionVertex) < 0.001){
            return child2;
        }

    }
    return 0;

}

void ActivePlan::addEdgeDirectionEvent() {

}

void ActivePlan::updateAtCurrentHeight(float currentHeight)
{
    foreach(Edge* edge, activePlan) {
        Vertex* v1 = edge->getVertex1();
        Vertex* v2 = edge->getVertex2();

        if(v1->getZ() < currentHeight) {
            std::cerr << "TODO";
        }
        if(v2->getZ() < currentHeight) {
            std::cerr << "TODO";
        }
    }
}

void ActivePlan::insert2Edges(Edge *old, Edge *new1, Edge *new2)
{
    bool found = false;
    for(std::vector<Edge*>::iterator i= activePlan.begin(); !found && i!= activePlan.end(); i++){
        if(*i==old){
            found = true;
            activePlan.erase(i);
            //activePlan.insert(i,new1);
            //activePlan.insert(i,new2);
            activePlan.push_back(new1);
            activePlan.push_back(new2);


            std::priority_queue<Event*, std::vector<Event*>, EventComparator>* priorityQueue = reconstruction3d->priorityQueue;
            std::vector<Event*> tempVector;
            while(!priorityQueue->empty()){
                Event* event = priorityQueue->top();
                priorityQueue->pop();

                if(event->isGeneralEvent()){
                    GeneralEvent* e = (GeneralEvent*) event;
                    for( std::set<Edge*, GeneralEvent::EdgePointerComparator>::iterator it= e->getEdges()->begin() ; it != e->getEdges()->end(); it++){
                        if(*it == old){

                           Edge * intersectingEdge = isIntersectionWithChildCorrect(e, old, new1, new2);
                           if(intersectingEdge!= 0){
                               e->getEdges()->erase(old);
                               e->getEdges()->insert(intersectingEdge);
                           } else {
                                if(!old->isValid()){
                                    std::cerr << " devrait pas etre possible mais... edges child must intersect eh"<< std::endl;
                                } else {
                                    std::cerr <<  "doevrait pas arriver grrrrr"<< std::endl;
                                }
                            }
                        }
                    }
                }

                tempVector.push_back(event);
            }

            foreach (Event* event, tempVector) {
                priorityQueue->push(event);
            }

        }
    }
    if(!found){
        std::cerr << " - old edge not found!!!" << std::endl;
    }
}

void ActivePlan::getActivePlanCopy(std::vector< std::vector< Edge* > >* copy) {
    std::vector< Edge* > currentLevel;
    foreach(Edge* e, activePlan) {
        Vertex* v1 = e->getVertex1();
        Vertex* v2 = e->getVertex2();
        currentLevel.push_back(new Edge(new Vertex(v1->getX(), v1->getY(), v1->getZ()), new Vertex(v2->getX(), v2->getY(), v2->getZ())));
    }
    copy->push_back(currentLevel);
}

void ActivePlan::removeInvalidEdges() {
    unsigned int size = activePlan.size();
    for(unsigned int i(0); i < size; ++i) {
        Edge* currentEdge = activePlan[i];
        if(!currentEdge->isValid()) {
            activePlan.erase(activePlan.begin() + i);
            i--;
            size--;
        }
    }
}
