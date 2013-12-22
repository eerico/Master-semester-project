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
    clustering(reconstruction3D);
    filteringInvalidEdges();

    if(edges.size() < 3) {
        return;
    }

    reconstruction3D->activePlan->checkConsistency();
    Chains chains(this, reconstruction3D->triangles, reconstruction3D->activePlan);
    chains.getChains(reconstruction3D->chainsDebug);
    chains.intraChainHandling();
    reconstruction3D->activePlan->checkConsistency();

    chains.interChainHandling();
    chains.getChains(reconstruction3D->chainsDebug2);
    reconstruction3D->activePlan->checkConsistency();

    reconstruction3D->activePlan->removeInvalidEdges();
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

    float delta1 = reconstruction3D->deltaHeight;
    float delta2(0.000001f);

    bool stop(false);

    std::vector<Event*> eventVectorTmp;
    while(!stop && (priorityQueue->size() > 0)){
        Event* event = priorityQueue->top();

        if(!event->isGeneralEvent()) {
            priorityQueue->pop();
            eventVectorTmp.push_back(event);
            continue;
        }

        GeneralEvent* generalEvent = (GeneralEvent*)(event);

        if (std::abs(generalEvent->getZ()- z) < delta1) {
            priorityQueue->pop();

            if(Utils::distance(generalEvent->getX(), generalEvent->getY(), x, y) < delta2){
                std::set<Edge*, EdgePointerComparator>* eventEdges = generalEvent->getEdges();
                for (std::set<Edge*, EdgePointerComparator>::iterator it = eventEdges->begin(); it != eventEdges->end(); ++it) {
                    edges.insert(*it);
                }
            } else {
                eventVectorTmp.push_back(event);
            }
        } else {
            stop = true;
        }
    }

    while(!eventVectorTmp.empty()) {
        Event* event = eventVectorTmp[eventVectorTmp.size() - 1];
        priorityQueue->push(event);
        eventVectorTmp.pop_back();
    }
}

void GeneralEvent::filteringInvalidEdges() {
    std::set<Edge*, EdgePointerComparator> copy;
    for (std::set<Edge*, EdgePointerComparator>::iterator it = edges.begin(); it != edges.end(); ++it) {
        Edge* edge = *it;
        if(edge->isValid()) {
            copy.insert(edge);
        }
    }
    edges.clear();

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
