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

    // create the vector where the computed triangle will be pushed
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
    activePlan->computeIntersections();
    activePlan->addEdgeDirectionEvent();
    activePlan->checkConsistency();

    // now we take every event and handle them
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

        // handle all event that are more or less at the current height (based on the clustering used
        // in the general event)
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

        // now we have handle all event at the current height

        //clear the piority queue
        while(!priorityQueue->empty()) {
            Event* event = priorityQueue->top();
            priorityQueue->pop();
            delete event;
        }

        // if there is more to do
        if(activePlan->size() > 2){
            // make the current active plan flat (all vertices
            // will be moved to the current height)
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

    //The reconstruction is done

    //reset to inital state
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        // move the iterator on the fiest direction plan for every profile
        currentVertex->getEdge2()->getProfile()->resetDirectionPlan();
        currentVertex = currentVertex->getNeighbor2();
    }

    // delete old data and
    // copy new triangles into the triangles to show
    // If an error has occured during the 3d reconstruction, we will
    // return the previous working 3D reconstruction
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
