#ifndef EDGEEVENT_H
#define EDGEEVENT_H

#include "Event.h"
#include <vector>
#include "../Geometry/Edge.h"

/**
 * @brief The EdgeEvent class
 * This class represents an edge event. An edge event occurs when a profile change its angle.
 * Every edge in the floor plan has a profile.
 *
 */
class EdgeEvent: public Event
{
public:
    /**
     * @brief EdgeEvent
     * @param z The height of the edge event
     * @param edge The edge associated with the profile
     */
    EdgeEvent(float z, Edge *edge);
    ~EdgeEvent();

    /**
     * @brief handle
     * Handle this edge event
     * @param reconstruction3D
     */
    void handle(Reconstruction3D* reconstruction3D);

    /**
     * @brief isGeneralEvent
     * @return false
     */
    bool isGeneralEvent();

    /**
     * @brief getFirstEdge
     * @return the first edge associated with this edge event
     */
    Edge* getFirstEdge();

    /**
     * @brief getEdges
     * @return all edge associated with this edge event
     */
    std::vector<Edge*>* getEdges();

    /**
     * @brief addEdge
     * Add an edge to this edge event
     * @param edge
     */
    void addEdge(Edge* edge);

private:
    // a vector that contain all edges associated with this edge event
    std::vector<Edge*> edges;

    /**
     * @brief addNewTriangle
     * Add a new triangle based on the three vertices
     * @param reconstruction3D
     * @param vertex1
     * @param vertex2
     * @param vertex3
     */
    void addNewTriangle(Reconstruction3D* reconstruction3D
                        , Vertex *vertex1, Vertex *vertex2, Vertex *vertex3);

};

#endif // EDGEEVENT_H
