 #include "Reconstruction3D.h"

const float Reconstruction3D::deltaHeight(0.0001f);

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles
                                   , std::vector<std::vector< std::vector< Edge* > > >* chainsDebug
                                   , std::vector<std::vector< std::vector< Edge* > > >* chainsDebug2
                                   , std::vector<std::vector<Edge *> > *activePlanDebug)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles)
    , chainsDebug(chainsDebug), chainsDebug2(chainsDebug2), activePlanDebug(activePlanDebug), currentHeight(0.0f)
{
    priorityQueue = new std::priority_queue<Event*, std::vector<Event*>, EventComparator>;
}

Reconstruction3D::~Reconstruction3D()
{
    delete priorityQueue;
    GeneralDestructorManager::deleteObject();
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

    while(!priorityQueue->empty()) {
        Event* event = priorityQueue->top();
        priorityQueue->pop();
        currentHeight = event->getZ();
        event->handle(this);

        delete event;

       if(!priorityQueue->empty()) {
            Event* nextEvent = priorityQueue->top();
            while((std::abs(nextEvent->getZ() - currentHeight) < deltaHeight) && !priorityQueue->empty()) {
                priorityQueue->pop();
                nextEvent->handle(this);
                delete nextEvent;

                if(!priorityQueue->empty()) {
                    nextEvent = priorityQueue->top();
                }
            }
        }
       //return;

        //clear the piority queue
        while(!priorityQueue->empty()) {
            Event* event = priorityQueue->top();
            priorityQueue->pop();
            delete event;
        }

        activePlan->updateAtCurrentHeight(currentHeight);
        activePlan->eliminateParallelNeighbor();
        activePlan->computeIntersections();
    }
}
