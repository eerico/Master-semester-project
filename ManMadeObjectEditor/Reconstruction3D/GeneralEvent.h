#ifndef GENERALEVENT_H
#define GENERALEVENT_H

#include "Event.h"
#include <vector>
#include <queue>
#include "EventComparator.h"
#include <set>
#include "../Geometry/Edge.h"

class Chains;

class GeneralEvent: public Event
{

public:
    GeneralEvent(float x, float y, float z);
    ~GeneralEvent();

    struct EdgePointerComparator {
        bool operator() (Edge* e1, Edge* e2) const
        {
            return e1 < e2;
        }
    };

    void handle(Reconstruction3D* reconstruction3D);
    bool isGeneralEvent();

    float getX();
    float getY();

    std::set<Edge*, EdgePointerComparator>* getEdges();
    void addEdge(Edge* edge);

private:
    float x;
    float y;

    std::set<Edge*, EdgePointerComparator> edges;

    void clustering(Reconstruction3D *reconstruction3D);
    void filteringInvalidEdges();
};

#endif // GENERALEVENT_H
