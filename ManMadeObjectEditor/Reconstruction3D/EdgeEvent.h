#ifndef EDGEEVENT_H
#define EDGEEVENT_H

#include "Event.h"
#include <vector>
#include "../Geometry/Edge.h"

class EdgeEvent: public Event
{
public:
    EdgeEvent(float z, Edge *edge);
    ~EdgeEvent();

    void handle(Reconstruction3D* reconstruction3D);
    bool isGeneralEvent();

    Edge* getFirstEdge();
    std::vector<Edge*>* getEdges();

    void addEdge(Edge* edge);

private:
    std::vector<Edge*> edges;
    void addNewTriangle(Reconstruction3D* reconstruction3D
                        , Vertex *vertex1, Vertex *vertex2, Vertex *vertex3);

};

#endif // EDGEEVENT_H
