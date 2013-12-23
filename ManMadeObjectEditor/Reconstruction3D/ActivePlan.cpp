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
                            if(intersection->getZ() > reconstruction3d->currentHeight + reconstruction3d->deltaHeight) {
                                if(reconstruction3d->priorityQueue->empty()) {
                                    if(isIntersectionCorrect(intersection, edge3)){
                                        intersection->addEdge(edge1);
                                        intersection->addEdge(edge2);
                                        intersection->addEdge(edge3);
                                        reconstruction3d->priorityQueue->push(intersection);
                                    }
                                } else {
                                    float currentLowestHeight = reconstruction3d->priorityQueue->top()->getZ();
                                    if((intersection->getZ() - currentLowestHeight) < reconstruction3d->deltaHeight) {
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
    Plan horizontalPlan (0.f,0.f, intersection->getZ());
    GeneralEvent * intersection1 = horizontalPlan.intersect3Plans(old->getDirectionPlan(), old->getVertex1()->getEdge1()->getDirectionPlan());
    GeneralEvent * intersection2 = horizontalPlan.intersect3Plans(old->getDirectionPlan(), old->getVertex2()->getEdge2()->getDirectionPlan());



    Vertex intersectionVertex(intersection->getX(),intersection->getY(), intersection->getZ());



    if(intersection1!=0){
        Vertex v1(intersection1->getX(),intersection1->getY(),intersection1->getZ());
        Edge edge(&v1, child1->getVertex2());
        if(edge.distanceXY(&intersectionVertex) < 0.001){
        return child1;
        }
    }
    if(intersection2!= 0){
        Vertex v2(intersection2->getX(),intersection2->getY(),intersection2->getZ());
        Edge edge(child2->getVertex1(), &v2);
        if(edge.distanceXY(&intersectionVertex) < 0.001){
            return child2;
        }

    }
    return 0;
}

void ActivePlan::updateAtCurrentHeight(float currentHeight)
{
    foreach(Edge* edge, activePlan) {
        if(!edge->isValid()) {
            continue;
        }

        Vertex* v1 = edge->getVertex1();
        Vertex* v2 = edge->getVertex2();
        Plan horizontalPlan (0.f, 0.f, currentHeight);

        if(v1->getZ() < currentHeight) {
            Plan* plan1 = v1->getEdge1()->getDirectionPlan();
            Plan* plan2 = edge->getDirectionPlan();

            GeneralEvent* intersection = horizontalPlan.intersect3Plans(plan1, plan2);
            if(intersection== 0) {
                if(edge->isParallel(v1->getEdge2())){
                    std::cerr << "parallel!! ";
                }
                std::cerr << "merde!"<< std::endl;
                if(plan1 == plan2) {
                    std::cerr << "merde encore plus!" << std::endl;
                    std::cerr << activePlan.size() << std::endl;
                }
            }


            Vertex vertex(intersection->getX(), intersection->getY(), intersection->getZ());
            addNewTriangle(v1, v2, &vertex);
            addNewTriangle(v1->getEdge1()->getVertex1(), v1, &vertex);

            v1->setX(vertex.getX());
            v1->setY(vertex.getY());
            v1->setZ(currentHeight);
        }
        if(v2->getZ() < currentHeight) {
            Plan* plan1 = edge->getDirectionPlan();
            Plan* plan2 = v2->getEdge2()->getDirectionPlan();

            GeneralEvent* intersection = horizontalPlan.intersect3Plans(plan1, plan2);
            if(intersection== 0) {
                if(edge->isParallel(v2->getEdge2())){
                    std::cerr << "parallel!! ";
                }
                std::cerr << "merde!"  << std::endl;
                if(plan1 == plan2) {
                    std::cerr << "merde encore plus!" << std::endl;
                    std::cerr << activePlan.size() << std::endl;
                }
            }

            Vertex vertex(intersection->getX(), intersection->getY(), intersection->getZ());
            addNewTriangle(v1, v2, &vertex);
            addNewTriangle(v2, v2->getEdge2()->getVertex2(), &vertex);

            v2->setX(vertex.getX());
            v2->setY(vertex.getY());
            v2->setZ(currentHeight);
        }
    }

    // check if the plans are really at the correct height
    foreach(Edge* edge, activePlan) {
        if(!edge->isValid()) {
            continue;
        }
        if(edge->getVertex1() != edge->getDirectionPlan()->getVertex()) {
            std::cerr << "plan Error" << std::endl;
            std::cerr << *edge->getVertex1() << " -> " << *edge->getDirectionPlan()->getVertex() << std::endl;
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

                bool toPush = true;


                if(event->isGeneralEvent()){
                    GeneralEvent* e = (GeneralEvent*) event;
                    for( std::set<Edge*, GeneralEvent::EdgePointerComparator>::iterator it= e->getEdges()->begin() ; it != e->getEdges()->end(); it++){
                        if(*it == old){

                            Edge * intersectingEdge = isIntersectionWithChildCorrect(e, old, new1, new2);
                            if(intersectingEdge!= 0){
                                e->getEdges()->erase(old);
                                e->getEdges()->insert(intersectingEdge);
                                break;
                            } else {
                                toPush = false;
                                if(!old->isValid()){
                                    std::cerr << " devrait pas etre possible mais... edges child must intersect eh"<< std::endl;
                                } else {
                                    std::cerr <<  "doevrait pas arriver grrrrr"<< std::endl;
                                }
                            }
                        }
                    }
                } else {
                    EdgeEvent* e = (EdgeEvent*) event;
                    std::vector<Edge*>* edges = e->getEdges();
                    unsigned int size = edges->size();
                    for(unsigned int i(0); i < size; ++i) {
                        Edge* edgeInVector = (*edges)[i];
                        if(edgeInVector == old) {
                            edges->erase(edges->begin() + i);
                            edges->push_back(new1);
                            edges->push_back(new2);
                            break;
                        }
                    }
                }
                if(toPush){
                    tempVector.push_back(event);
                }
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
        if(!currentEdge->isValid() || (currentEdge->getVertex1() == currentEdge->getVertex2())) {
            activePlan.erase(activePlan.begin() + i);
            i--;
            size--;
        }
    }
}

// a utiliser quand tout est au meme niveau !!
// il faudrait faire une check de sureter histoire d etre bien sure que quand on l appele c est effectivement le cas..
void ActivePlan::eliminateParallelNeighbor()
{
    float currentLevel = reconstruction3d->currentHeight;
    foreach(Edge* edge, activePlan) {
        Vertex* v2 = edge->getVertex2();
        if(std::abs(v2->getZ() - currentLevel) > 2.1f * reconstruction3d->deltaHeight) {
            std::cerr << "Error FloorPlan not flat: " << std::abs(v2->getZ() - currentLevel) << std::endl;
        }
    }

    unsigned int size = activePlan.size();
    for(unsigned int i(0); i < size; ++i) {
        Edge* currentEdge = activePlan[i];
        Edge* nextEdge = currentEdge->getVertex2()->getEdge2();

        Plan* p1 = currentEdge->getDirectionPlan();
        Plan* p2 = nextEdge->getDirectionPlan();

        float nx1, ny1, nz1;
        float nx2, ny2, nz2;
        p1->getNormal(nx1, ny1, nz1);
        p2->getNormal(nx2, ny2, nz2);

        float dotProduct = nx1*nx2 + ny1*ny2 + nz1*nz2;

        if(currentEdge->isParallel(nextEdge) || std::abs(std::abs(dotProduct) - 1.0f) < 0.0001f) {
            activePlan.erase(activePlan.begin() + i);
            i--;
            size--;

            Vertex* nextV1 = nextEdge->getVertex1();
            Vertex* currentV1 = currentEdge->getVertex1();

            //add a triangle to avoir holes because of precision problem
            Vertex* nextV2 = nextEdge->getVertex2();
            if((nextV1->distance(currentV1) > 0.00001f)
                    && (nextV1->distance(nextV2) > 0.00001f)
                    && (currentV1->distance(nextV2) > 0.00001f)) {
                addNewTriangle(currentV1, nextV1, nextV2);
            }

            nextV1->setX(currentV1->getX());
            nextV1->setY(currentV1->getY());
            nextV1->setZ(currentV1->getZ());

            nextV1->setEdge1(currentV1->getEdge1());
            nextV1->setNeighbor1(currentV1->getNeighbor1());

            nextEdge->setDirectionPlan(currentEdge->getDirectionPlan());
            nextEdge->getDirectionPlan()->setVertex(nextEdge->getVertex1());

            currentV1->getNeighbor1()->setNeighbor2(nextV1);

            currentV1->getEdge1()->setVertex2(nextV1);
        }
    }
}

float ActivePlan::size()
{
    return activePlan.size();
}

void ActivePlan::checkConsistency()
{
    foreach(Edge* edge, activePlan) {
        if(!edge->isValid()) {
            continue;
        }
        Vertex* v = edge->getVertex1();
        Vertex* n1 = v->getNeighbor1();
        Vertex* n2 = v->getNeighbor2();
        Edge* e1 = v->getEdge1();
        Edge* e2 = v->getEdge2();

        if(n1 != e1->getVertex1()) {
            std::cerr << "Error 1" << std::endl;
        }
        if(n2 != e2->getVertex2()) {
            std::cerr << "Error 2" << std::endl;
        }
        if(edge != e2) {
            std::cerr << "Error 3" << std::endl;
        }
        if(e2 != v->getEdge2()) {
            std::cerr << "Error 4" << std::endl;
        }
        if(e2 != n2->getEdge1()) {
            std::cerr << "Error 5" << std::endl;
        }
        if(e2->getVertex1() != v) {
            std::cerr << "Error 6" << std::endl;
        }
        if(e2->getVertex2() != n2) {
            std::cerr << "Error 7" << std::endl;
        }

        if(e1 != v->getEdge1()) {
            std::cerr << "Error 8" << std::endl;
        }
        if(e1 != n1->getEdge2()) {
            std::cerr << "Error 10 " << std::endl;
        }
        if(e1->getVertex1() != n1) {
            std::cerr << "Error 11" << std::endl;
        }
        if(e1->getVertex2() != v) {
            std::cerr << "Error 12" << std::endl;
        }
        /*if(n1 == n2) {
            std::cerr << "Error 13" << std::endl;
        }*/
    }
}

void ActivePlan::addEdgeDirectionEvent()
{
    std::vector<EdgeEvent*> edgeEvents;
    foreach(Edge* currentEdge, activePlan) {
        Vertex* currentProfileVertex = currentEdge->getProfile()->getProfileVertexIterator();
        currentProfileVertex = currentProfileVertex->getNeighbor2();
        while(currentProfileVertex != 0 && currentProfileVertex->getNeighbor2() != 0) {
            if(currentProfileVertex->getY() > reconstruction3d->currentHeight + reconstruction3d->deltaHeight) {

                bool found = false;
                foreach(EdgeEvent* event, edgeEvents) {
                    if(event->getFirstEdge()->getProfile() == currentEdge->getProfile()) {
                        if(std::abs(event->getZ() - currentProfileVertex->getY()) < 0.000001f ) {
                            event->addEdge(currentEdge);
                            found = true;
                            break;
                        }
                        // on peut eviter de mettre les event trop haut
                    }
                }

                if(!found) {
                    EdgeEvent* edgeDirectionEvent = new EdgeEvent(currentProfileVertex->getY(), currentEdge);
                    edgeEvents.push_back(edgeDirectionEvent);
                }
            }
            currentProfileVertex = currentProfileVertex->getNeighbor2();
        }
    }

    foreach(EdgeEvent* event, edgeEvents) {
        reconstruction3d->priorityQueue->push(event);
    }
}


void ActivePlan::addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) {
    qglviewer::Vec* triangleVertex1 = new qglviewer::Vec(vertex1->getX(), vertex1->getY(), vertex1->getZ());
    qglviewer::Vec* triangleVertex2 = new qglviewer::Vec(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    qglviewer::Vec* triangleVertex3 = new qglviewer::Vec(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    reconstruction3d->triangles->push_back(triangleVertex1);
    reconstruction3d->triangles->push_back(triangleVertex2);
    reconstruction3d->triangles->push_back(triangleVertex3);
}


