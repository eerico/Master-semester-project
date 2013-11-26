#include "Reconstruction3D.h"

//#define DEBUG

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles), minimumHeight(0.0f)
{
    priorityQueue = new std::priority_queue<Intersection, std::vector<Intersection>, IntersectionComparator>;
}

Reconstruction3D::~Reconstruction3D()
{
    delete priorityQueue;
}

void Reconstruction3D::reconstruct()
{
    //set the current ActivePlan
    activePlan = new ActivePlan(floorPlan, floorPlanSize, triangles);
    //compute the current direction plan
    activePlan->computeDirectionPlan();

    //main loop
    addEdgeDirectionEvent();

    unsigned int numberMaxIteration = 100;
    unsigned int numberIteration = 0;
    do{
        computeIntersection();
        while((priorityQueue->size() > 0)) {
            Intersection event = priorityQueue->top();
            priorityQueue->pop();
            handleEvent(event);
        }
        numberIteration++;
    // maybe after the algorithm we still have some holes thus the number of valid
    // edges is more than 0 thus we can run again the algorithm. To be sure that it will terminate
    // we impose a maximum number of iteration
    }while(activePlan->numberValidEdge() > 0 && numberIteration < numberMaxIteration);

    //reset to inital state
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        currentVertex->getEdge2()->getProfile()->resetDirectionPlan();
        currentVertex = floorPlan->getNeighbor2();
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
             */
            Edge* edge = intersection.edge;

            edgeDirectionHandling(intersection);

            Profile* profile = edge->getProfile();
            profile->nextDirectionPlan();

            computeIntersection();

            break;
        }
        case General:
        {

            /*std::cerr << "intersection: " << intersection.x << ", " << intersection.y << ", " << intersection.z << std::endl;
            std::cerr << "with edges: " << std::endl;
            foreach(Edge* e, *intersection.edgeVector) {
                std::cerr << "    " << *e << std::endl;
            }*/


            if(!eventClustering(intersection)) {
                return;
            }

            if(intersection.edgeVector->size() < 3) {
                return;
            }



            std::cerr << "intersection: " << intersection.x << ", " << intersection.y << ", " << intersection.z << std::endl;
            std::cerr << "with edges: " << std::endl;
            foreach(Edge* e, *intersection.edgeVector) {
                std::cerr << "    " << *e << std::endl;
            }


            Chains* chainList = new Chains(&intersection, triangles, activePlan);
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

                computeIntersection();
            }

            // juste pour debug
            #ifdef DEBUG
                activePlan = new ActivePlan(intersection.z, activePlan, triangles);
            #endif
            //std::cerr << "......................................................................." << std::endl;
            break;
        }
    }
}


// return false si l intersection n est pas valid
bool Reconstruction3D::eventClustering(Intersection& intersection)
{
    float z(intersection.z);

    float delta1(0.0001f);
    float delta2(0.000001f);

    bool stop(false);

    std::vector<Edge*>* intersectionEdges = intersection.edgeVector;

    if(!activePlan->filteringInvalidEvent(intersection)) {
        return false;
    }

    std::vector<Intersection> eventVectorTmp;
    while(!stop && (priorityQueue->size() > 0)){
        Intersection event = priorityQueue->top();

        // if the current intersection (event) is invalid, we skip to the next one
        if(!activePlan->filteringInvalidEvent(event)) {
            priorityQueue->pop();
            continue;
        }

        if (std::abs(event.z - z) < delta1) {
            priorityQueue->pop();

            if(Utils::distance(event.x, event.y, intersection.x, intersection.y) < delta2){
                std::vector<Edge*>* eventEdges = event.edgeVector;

                for(unsigned int i(0); i < eventEdges->size(); ++i) {
                    Edge* edgeToAdd = (*eventEdges)[i];
                    if(!isEdgeInVector(edgeToAdd, intersectionEdges)) {
                        intersectionEdges->push_back(edgeToAdd);
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


void Reconstruction3D::addEdgeDirectionEvent() ////////////////////////
{
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        Edge* currentEdge = currentVertex->getEdge2();
        Vertex* currentProfileVertex = currentEdge->getProfile()->getProfileVertex();
        currentProfileVertex = currentProfileVertex->getNeighbor2();

        while(currentProfileVertex != 0 && currentProfileVertex->getNeighbor2() != 0) {
            Intersection edgeDirectionEvent;
            edgeDirectionEvent.edge = currentEdge;
            edgeDirectionEvent.eventType = EdgeDirection;
            edgeDirectionEvent.z = currentProfileVertex->getY();
            priorityQueue->push(edgeDirectionEvent);
            currentProfileVertex = currentProfileVertex->getNeighbor2();
        }
        currentVertex = floorPlan->getNeighbor2();
    }
}

void Reconstruction3D::edgeDirectionHandling(Intersection &intersection) //////////////////
{

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
