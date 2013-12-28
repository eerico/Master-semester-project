#ifndef GENERALEVENT_H
#define GENERALEVENT_H

#include "Event.h"
#include <vector>
#include <queue>
#include "EventComparator.h"
#include <set>
#include "../Geometry/Edge.h"

class Chains;

/**
 * @brief The GeneralEvent class
 * This class represents a general event. A general event occurs when three direction plan intersect.
 * Every edge has a direction plan associated computed with their profile
 *
 */
class GeneralEvent: public Event
{

public:
    /**
     * @brief GeneralEvent
     * @param x The x coordinate of the intersection
     * @param y The y coordinate of the intersection
     * @param z The z coordinate of the intersection
     */
    GeneralEvent(float x, float y, float z);
    ~GeneralEvent();

    /**
     * @brief The EdgePointerComparator struct
     * Used to construct the set of edges
     */
    struct EdgePointerComparator {
        bool operator() (Edge* e1, Edge* e2) const
        {
            return e1 < e2;
        }
    };

    /**
     * @brief handle
     * Handle this general event
     * @param reconstruction3D
     */
    void handle(Reconstruction3D* reconstruction3D);

    /**
     * @brief isGeneralEvent
     * @return True
     */
    bool isGeneralEvent();

    float getX();
    float getY();

    /**
     * @brief getEdges
     * @return All edges involved in this general event
     */
    std::set<Edge*, EdgePointerComparator>* getEdges();

    /**
     * @brief addEdge
     * Add an edge to this general event
     * @param edge
     */
    void addEdge(Edge* edge);

private:
    // the x coordinate of this intersection
    float x;
    // the y coordinate of this intersection
    float y;

    // all edges involved in this general event (all edge involved in the intersection)
    std::set<Edge*, EdgePointerComparator> edges;

    /**
     * @brief clustering
     * Clustering algorithm used to find all general event that are enough close to this
     * one and merge them
     * @param reconstruction3D
     */
    void clustering(Reconstruction3D *reconstruction3D);

    /**
     * @brief filteringInvalidEdges
     * Remove all invalid edges
     */
    void filteringInvalidEdges();
};

#endif // GENERALEVENT_H
