 #include "Reconstruction3D.h"

const float Reconstruction3D::deltaHeight(0.0001f);

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles
                                   , std::vector<std::vector< std::vector< Edge* > > >* chainsDebug
                                   , std::vector<std::vector< std::vector< Edge* > > >* chainsDebug2
                                   , std::vector<std::vector<Edge *> > *activePlanDebug)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), trianglesToShow(triangles)
    , chainsDebug(chainsDebug), chainsDebug2(chainsDebug2), activePlanDebug(activePlanDebug), currentHeight(0.0f)
{
    priorityQueue = new std::priority_queue<Event*, std::vector<Event*>, EventComparator>;

    this->triangles = new std::vector<qglviewer::Vec* >;

    error = new bool;
    (*error) = false;
}

Reconstruction3D::~Reconstruction3D()
{
    delete priorityQueue;
    GeneralDestructorManager::deleteObject();
    delete error;
}

void Reconstruction3D::reconstruct()
{
    // there is nothing to reconstruct
    if (floorPlanSize == 0) {
        return;
    }

    activePlan = new ActivePlan(floorPlan, floorPlanSize, this);
    activePlan->eliminateParallelNeighbor();
    activePlan->computeIntersections(); // mettre seulement ceux qui sont a la bonne heuteur
    activePlan->addEdgeDirectionEvent();
    activePlan->checkConsistency();


    while(!priorityQueue->empty()) {
        Event* event = priorityQueue->top();
        priorityQueue->pop();
        currentHeight = event->getZ();
        event->handle(this);

        delete event;

        if((*error)) {
            errorHandling();
            return;
        }

        if(!priorityQueue->empty()) {
            Event* nextEvent = priorityQueue->top();
            while((std::abs(nextEvent->getZ() - currentHeight) < deltaHeight) && !priorityQueue->empty()) {
                priorityQueue->pop();
                nextEvent->handle(this);
                delete nextEvent;

                if((*error)) {
                    errorHandling();
                    return;
                }

                if(!priorityQueue->empty()) {
                    nextEvent = priorityQueue->top();
                }
            }
        }

        //clear the piority queue
        while(!priorityQueue->empty()) {
            Event* event = priorityQueue->top();
            priorityQueue->pop();
            delete event;
        }

        if(activePlan->size() > 2){
            activePlan->updateAtCurrentHeight(currentHeight);
            if((*error)) {
                errorHandling();
                return;
            }
            activePlan->eliminateParallelNeighbor();
            activePlan->computeIntersections();
            activePlan->addEdgeDirectionEvent();
        }
    }

    //reset to inital state
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        currentVertex->getEdge2()->getProfile()->resetDirectionPlan();
        currentVertex = currentVertex->getNeighbor2();
    }

    // delete old data and
    // copy new triangles into the triangles to show
    unsigned int trianglesSize = trianglesToShow->size();
    for(unsigned int i(0); i < trianglesSize; ++i) {
        delete (*trianglesToShow)[i];
    }
    trianglesToShow->clear();

    trianglesSize = triangles->size();
    for(unsigned int i(0); i < trianglesSize; ++i) {
        trianglesToShow->push_back((*triangles)[i]);
    }
}

void Reconstruction3D::errorHandling()
{
    //reset to inital state
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        currentVertex->getEdge2()->getProfile()->resetDirectionPlan();
        currentVertex = currentVertex->getNeighbor2();
    }

    //clear priority queue
    while(!priorityQueue->empty()) {
        Event* event = priorityQueue->top();
        priorityQueue->pop();
        delete event;
    }

    // clear triangles
    unsigned int trianglesSize = triangles->size();
    for(unsigned int i(0); i < trianglesSize; ++i) {
        delete (*triangles)[i];
    }
    triangles->clear();
}
