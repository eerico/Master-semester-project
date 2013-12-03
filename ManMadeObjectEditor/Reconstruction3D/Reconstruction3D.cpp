#include "Reconstruction3D.h"

//#define DEBUG

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles
                                   , std::vector<std::vector< std::vector< Edge* > > >* chainsDebug
                                   , std::vector<std::vector< std::vector< Edge* > > >* chainsDebug2
                                   , std::vector<std::vector<Edge *> > *activePlanDebug)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles), minimumHeight(0.0f)
    , chainsDebug(chainsDebug), chainsDebug2(chainsDebug2), activePlanDebug(activePlanDebug)
{
    priorityQueue = new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>;
}

Reconstruction3D::~Reconstruction3D()
{
    delete priorityQueue;
}

void Reconstruction3D::reconstruct()
{
    //std::cerr << "RECONSTRUCT" << std::endl;

    // there is nothing to reconstruct
    if (floorPlanSize == 0) {
        return;
    }

    //set the current ActivePlan
    activePlan = new ActivePlan(floorPlan, floorPlanSize, triangles);
    //compute the current direction plan
    activePlan->computeDirectionPlan();

    //main loop
    addEdgeDirectionEvent();

    computeIntersection();
    while((priorityQueue->size() > 0)) {
        Intersection event = priorityQueue->top();
        priorityQueue->pop();
        handleEvent(event);
    }

    /*if (activePlan->numberValidEdge() > 0) {
        activePlan->fillHoles();
    }*/

    //std::cerr << activePlan->numberValidEdge() << std::endl;
    //std::cerr << activePlan->getPlan()->size() << std::endl;
    //activePlan->print(true);

    //reset to inital state
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        currentVertex->getEdge2()->getProfile()->resetDirectionPlan();
        currentVertex = currentVertex->getNeighbor2();
    }
}

void Reconstruction3D::computeIntersection()
{

    std::vector< Edge* >* plans = activePlan->getPlan();

    unsigned int numberEdges = plans->size();

    for(unsigned int i(0); i < numberEdges; ++i) {
        Edge* edge2 = (*plans)[i];
        Edge* edge3 = edge2->getVertex2()->getEdge2();

        foreach(Edge* edge1, (*plans)) {
            if(edge1 == edge2 || edge1 == edge3) {
                continue;
            }
            Intersection intersection = intersect(edge1, edge2, edge3);

            if(intersection.eventType != NoIntersection && intersection.z > minimumHeight) {
                priorityQueue->push(intersection);
            }
        }
    }

}

Intersection Reconstruction3D::intersect(Edge *edge1, Edge *edgeNeighbor1, Edge *edgeNeighbor2) ////////////////////////
{
    Plan* plan1 = edge1->getDirectionPlan();
    Plan* plan2 = edgeNeighbor1->getDirectionPlan();
    Plan* plan3 = edgeNeighbor2->getDirectionPlan();

    Intersection intersection = plan1->intersect3Plans(plan2, plan3);
    intersection.edgeVector = new std::vector< Edge* >;
    intersection.edgeVector->push_back(edge1);
    intersection.edgeVector->push_back(edgeNeighbor1);
    intersection.edgeVector->push_back(edgeNeighbor2);

    return intersection;
}

void Reconstruction3D::handleEvent(Intersection& intersection) ////////////////////////////////
{
    switch(intersection.eventType){
        case EdgeDirection:
        {

            /*
             * TODO: Le probleme ici c est que un profil peut etre le meme sur different edge
             * et donc on doit pas faire le nextDirectionPlan pour chaque edge qui on le meme profile
             * mais pour que ceux qui ont des profiles differents. Une idée serait de prendre tous les edges
             * qui on le meme profiles et d appliquer les changement une seul fois et des creer pour chacun de ses edges
             * les triangles
             *
             *
             * Attention, il faut update le direction plan
             */

            if(!intersection.edge->isValid()) {
                return;
            }

            edgeDirectionEventClustering(intersection);

            edgeDirectionHandling(intersection);

            std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>* priorityQueueTmp
                    = new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>;

            //remove the old computed intersection
            while(!priorityQueue->empty()) {
                Intersection intersection = priorityQueue->top();
                // we save the edge direction event
                if(intersection.eventType == EdgeDirection) {
                    priorityQueueTmp->push(intersection);
                }
                priorityQueue->pop();
            }
            delete priorityQueue;
            priorityQueue = priorityQueueTmp;

            minimumHeight = intersection.z + 0.000001f;

            computeIntersection();

            break;
        }
        case General:
        {
            //return;
            std::cerr << "before:" << std::endl;
            std::cerr << "intersection: " << intersection.x << ", " << intersection.y << ", " << intersection.z << std::endl;
            std::cerr << "with edges: " << std::endl;
            foreach(Edge* e, *intersection.edgeVector) {
                std::cerr << "    " << *e << std::endl;
            }


            if(!generalEventClustering(intersection)) {
                std::cerr << "---INVALID" << std::endl;
                return;
            }

            activePlan->filteringInvalidEvent2(intersection);

            if(intersection.edgeVector->size() < 3) {
                std::cerr << "---<3" << std::endl;
                return;
            }

            std::cerr << "after:" << std::endl;
            std::cerr << "intersection: " << intersection.x << ", " << intersection.y << ", " << intersection.z << std::endl;
            std::cerr << "with edges: " << std::endl;
            foreach(Edge* e, *intersection.edgeVector) {
                std::cerr << "    " << *e << std::endl;
            }


            Chains* chainList = new Chains(&intersection, triangles, activePlan);

            chainList->getChains(chainsDebug);

            chainList->intraChainHandling();

            activePlan->removeInvalidEdges();

            if(chainList->interChainHandling()){
                // We have updated the current active plan by insterting new edges into it,
                // we will thus create a new active plan, delete the old one and recompute the
                // intersection for the new active plan (only the itersection above the current one,
                // we thus use a variable to control this behaviour, the variable minimumHeight)
                ActivePlan* oldActivePlan = activePlan;
                bool chainSizeOne;
                activePlan = new ActivePlan(oldActivePlan, triangles, chainSizeOne);

                // It can happen that a chain of size 1 is created. We just ignore it.
                if(!chainSizeOne) {
                    delete oldActivePlan;
                    minimumHeight = intersection.z + 0.000001f; //we use a small delta to ensure that the next intersection will be above the current one
                } else {
                    activePlan = oldActivePlan;
                    return;
                }

                // the inter chain algorithm has inserted new edges. We thus have to recompute the active plan,
                // recompute the intersection for this new active plan and handle these new intersections

                //remove the old computed intersection
                while(!priorityQueue->empty()) {
                    priorityQueue->pop();
                }

                addEdgeDirectionEvent();
                computeIntersection();
            }

            chainList->getChains(chainsDebug2);

            activePlan->getActivePlanCopy(activePlanDebug);

            break;
        }
    }
}


// return false si l intersection n est pas valid
bool Reconstruction3D::generalEventClustering(Intersection& intersection)
{
    float z(intersection.z);

    float delta1(0.0001f);
    float delta2(0.000001f);

    bool stop(false);

    std::vector<Edge*>* intersectionEdges = intersection.edgeVector;

    if(!activePlan->filteringInvalidEvent(intersection)) {
        std::cerr << "--FIltering invalid event" << std::endl;
        return false;
    }

    std::vector<Intersection> eventVectorTmp;
    while(!stop && (priorityQueue->size() > 0)){
        Intersection event = priorityQueue->top();

        if(event.eventType == EdgeDirection) {
            priorityQueue->pop();
            eventVectorTmp.push_back(event);
            continue;
        }

        // if the current intersection (event) is invalid, we skip to the next one
        /*if(!activePlan->filteringInvalidEvent(event)) {
            std::cerr << "  invalid event:" << std::endl;
            std::cerr << "  intersection: " << intersection.x << ", " << intersection.y << ", " << intersection.z << std::endl;
            std::cerr << "  with edges: " << std::endl;
            foreach(Edge* e, *intersection.edgeVector) {
                std::cerr << "      " << *e << std::endl;
            }
            priorityQueue->pop();
            continue;
        }*/

        if (std::abs(event.z - z) < delta1) {
            priorityQueue->pop();

            if(Utils::distance(event.x, event.y, intersection.x, intersection.y) < delta2){
                std::vector<Edge*>* eventEdges = event.edgeVector;

                for(unsigned int i(0); i < eventEdges->size(); ++i) {
                    Edge* edgeToAdd = (*eventEdges)[i];
                    if(edgeToAdd->isValid()) {
                        if(!isEdgeInVector(edgeToAdd, intersectionEdges)) {
                            intersectionEdges->push_back(edgeToAdd);
                        }
                    }
                }
            } else {
                eventVectorTmp.push_back(event);
            }
        } else {
            stop = true;
        }
    }

    while(!eventVectorTmp.empty()) {
        Intersection event = eventVectorTmp[eventVectorTmp.size() - 1];
        priorityQueue->push(event);
        eventVectorTmp.pop_back();
    }

    return true;
}

void Reconstruction3D::edgeDirectionEventClustering(Intersection& intersection)
{

    std::vector<Edge*>* edgeDirectionEdges = new std::vector<Edge*>;
    edgeDirectionEdges->push_back(intersection.edge);
    Profile* currentProfile = intersection.edge->getProfile();

    bool stop(false);
    float delta1(0.0001f);
    float z(intersection.z);

    std::vector<Intersection> eventVectorTmp;
    while(!stop && (priorityQueue->size() > 0)){
        Intersection event = priorityQueue->top();

        if(event.eventType == General) {
            priorityQueue->pop();
            continue;
        }

        Edge* eventEdge = event.edge;

        if(!eventEdge->isValid()) {
            priorityQueue->pop();
            continue;
        }

        if (std::abs(event.z - z) < delta1) {
            priorityQueue->pop();
            if(eventEdge->getProfile() == currentProfile){
                edgeDirectionEdges->push_back(eventEdge);
            } else {
                eventVectorTmp.push_back(event);
            }
        } else {
            stop = true;
        }
    }

    while(!eventVectorTmp.empty()) {
        Intersection event = eventVectorTmp[eventVectorTmp.size() - 1];
        priorityQueue->push(event);
        eventVectorTmp.pop_back();
    }
    intersection.edgeVector = edgeDirectionEdges;
}


void Reconstruction3D::addEdgeDirectionEvent() ////////////////////////
{
    std::vector< Edge* >* edges = activePlan->getPlan();
    foreach(Edge* currentEdge, *edges) {
        Vertex* currentProfileVertex = currentEdge->getProfile()->getProfileVertexIterator();
        currentProfileVertex = currentProfileVertex->getNeighbor2();
        while(currentProfileVertex != 0 && currentProfileVertex->getNeighbor2() != 0) {
            if(currentProfileVertex->getY() > minimumHeight) {
                Intersection edgeDirectionEvent;
                edgeDirectionEvent.edge = currentEdge;
                edgeDirectionEvent.eventType = EdgeDirection;
                edgeDirectionEvent.z = currentProfileVertex->getY();
                priorityQueue->push(edgeDirectionEvent);
            }
            currentProfileVertex = currentProfileVertex->getNeighbor2();
        }
    }
}

void Reconstruction3D::edgeDirectionHandling(Intersection &intersection) //////////////////
{
    std::vector< Edge* >* edges = intersection.edgeVector;
    Plan horizontalPlan(0.0f, 0.0f, intersection.z, 0.0f, 0.0f, 1.0f);

    foreach(Edge* edge, *edges) {
        // previousEdge(plan1) - currentEdge(currentPlan) - nextEdge(plan2)
        Vertex* vertex1 = edge->getVertex1();
        Vertex* vertex2 = edge->getVertex2();

        Plan* plan1 = vertex1->getEdge1()->getDirectionPlan();
        Plan* currentPlan = edge->getDirectionPlan();
        Plan* plan2 = vertex2->getEdge2()->getDirectionPlan();

        Intersection newIntersection1 = horizontalPlan.intersect3Plans(plan1, currentPlan);

        Vertex newIntersectionVertex1(newIntersection1.x, newIntersection1.y, newIntersection1.z);
        if (std::abs(vertex1->distance(&newIntersectionVertex1)) > 0.0001f) {
            addNewTriangle(vertex1, vertex2, &newIntersectionVertex1);
            addNewTriangle(vertex1->getEdge1()->getVertex1(), vertex1, &newIntersectionVertex1);
        }

        vertex1->setX(newIntersection1.x);
        vertex1->setY(newIntersection1.y);
        vertex1->setZ(newIntersection1.z);

        Intersection newIntersection2 = horizontalPlan.intersect3Plans(currentPlan, plan2);

        Vertex newIntersectionVertex2(newIntersection2.x, newIntersection2.y, newIntersection2.z);
        if (std::abs(vertex2->distance(&newIntersectionVertex2)) > 0.0001f) {
            addNewTriangle(vertex1, vertex2, &newIntersectionVertex2);
            addNewTriangle(vertex2, vertex2->getEdge2()->getVertex2(), &newIntersectionVertex2);
        }

        vertex2->setX(newIntersection2.x);
        vertex2->setY(newIntersection2.y);
        vertex2->setZ(newIntersection2.z);
    }

    intersection.edge->getProfile()->nextDirectionPlan();

    foreach(Edge* edge, *edges) {
        edge->getDirectionPlan()->computePlanNormal();
    }
}

bool Reconstruction3D::isEdgeInVector(Edge *edge, std::vector<Edge *> *vector) {
    unsigned int size = vector->size();
    Edge* currentEdge;
    for(unsigned int i(0); i < size; ++i) {
        currentEdge = (*vector)[i];
        if(currentEdge == edge) {
            return true;
        }
    }
    return false;
}

void Reconstruction3D::addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) {
    qglviewer::Vec* triangleVertex1 = new qglviewer::Vec(vertex1->getX(), vertex1->getY(), vertex1->getZ());
    qglviewer::Vec* triangleVertex2 = new qglviewer::Vec(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    qglviewer::Vec* triangleVertex3 = new qglviewer::Vec(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    triangles->push_back(triangleVertex1);
    triangles->push_back(triangleVertex2);
    triangles->push_back(triangleVertex3);
}
