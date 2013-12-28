#include "GeneralEvent.h"
#include "Chains.h"
#include "Reconstruction3D.h"



GeneralEvent::GeneralEvent(float x, float y, float z)
    :Event(z), x(x), y(y)
{
}

GeneralEvent::~GeneralEvent()
{

}

void GeneralEvent::handle(Reconstruction3D* reconstruction3D)
{
    // first apply the clustering to find all similar general event
    clustering(reconstruction3D);
    //remove all invalid edges
    filteringInvalidEdges();

    // if after removing all invalid edges we have less than 3 edges, we cannot have an intersection
    if(edges.size() < 3) {
        return;
    }

    // check if everything goes well
    reconstruction3D->activePlan->checkConsistency();

    //construct the chains
    Chains chains(this, reconstruction3D->triangles, reconstruction3D->activePlan);
    //get the chains information to allow the user to see it
    chains.getChains(reconstruction3D->chainsDebug);

    // apply the intra chain algorithm
    chains.intraChainHandling();
    // check if everything goes well
    reconstruction3D->activePlan->checkConsistency();

    //apply the inter chain algorithm
    chains.interChainHandling();
    //get the chains information to allow the user to see it
    chains.getChains(reconstruction3D->chainsDebug2);

    //remove invalid edges
    reconstruction3D->activePlan->removeInvalidEdges();
    // check if everything goes well
    reconstruction3D->activePlan->checkConsistency();
    // construct a copy of the current cative plan to allros the user to see it
    reconstruction3D->activePlan->getActivePlanCopy(reconstruction3D->activePlanDebug);
}

bool GeneralEvent::isGeneralEvent() {
    return true;
}

float GeneralEvent::getX()
{
    return x;
}

float GeneralEvent::getY()
{
    return y;
}

void GeneralEvent::clustering(Reconstruction3D* reconstruction3D)
{
    std::priority_queue<Event*, std::vector<Event*>, EventComparator>* priorityQueue = reconstruction3D->priorityQueue;

    // the clustering will find all general event that are enough similar to this one. We have thus to cluster
    // in the height direction and then in the xy direction

    // threshold for the clustering in the height direction
    float delta1 = reconstruction3D->deltaHeight;
    // threshold for the clustering in the xy distance
    float delta2(0.000001f);

    bool stop(false);

    // In order to go through the priority queue, we have to pop the event. We use this vector to save
    // the event that are not enough similar and then push them back in the priority queue
    std::vector<Event*> eventVectorTmp;
    while(!stop && (priorityQueue->size() > 0)){
        Event* event = priorityQueue->top();

        // if the event is not a general event, we dont cluster it
        if(!event->isGeneralEvent()) {
            priorityQueue->pop();
            eventVectorTmp.push_back(event);
            continue;
        }

        GeneralEvent* generalEvent = (GeneralEvent*)(event);

        // do the clustering in the heigh direction
        if (std::abs(generalEvent->getZ()- z) < delta1) {
            priorityQueue->pop();

            // do the clustering for the xy distance
            if(Utils::distance(generalEvent->getX(), generalEvent->getY(), x, y) < delta2){
                // merge the two set of edges
                std::set<Edge*, EdgePointerComparator>* eventEdges = generalEvent->getEdges();
                for (std::set<Edge*, EdgePointerComparator>::iterator it = eventEdges->begin(); it != eventEdges->end(); ++it) {
                    edges.insert(*it);
                }
            } else {
                // the event is not in the cluster thus save it
                eventVectorTmp.push_back(event);
            }
        } else {
            stop = true;
        }
    }

    // put back all event that have not been clustered
    while(!eventVectorTmp.empty()) {
        Event* event = eventVectorTmp[eventVectorTmp.size() - 1];
        priorityQueue->push(event);
        eventVectorTmp.pop_back();
    }
}

void GeneralEvent::filteringInvalidEdges() {
    // put all valid edge into the copy set
    std::set<Edge*, EdgePointerComparator> copy;
    for (std::set<Edge*, EdgePointerComparator>::iterator it = edges.begin(); it != edges.end(); ++it) {
        Edge* edge = *it;
        if(edge->isValid()) {
            copy.insert(edge);
        }
    }
    edges.clear();

    // put all valid edge in the edges set
    for (std::set<Edge*, EdgePointerComparator>::iterator it = copy.begin(); it != copy.end(); ++it) {
        Edge* edge = *it;
        edges.insert(edge);
    }
}

std::set<Edge*, GeneralEvent::EdgePointerComparator>* GeneralEvent::getEdges() {
    return &edges;
}

void GeneralEvent::addEdge(Edge *edge)
{
    edges.insert(edge);
}
