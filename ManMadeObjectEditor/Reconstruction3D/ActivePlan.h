#ifndef ACTIVEPLAN_H
#define ACTIVEPLAN_H

#include "../Geometry/Vertex.h"
//#include "Reconstruction3D.h"
#include "Plan.h"
#include "GeneralEvent.h"
#include "EdgeEvent.h"
#include <vector>

class Reconstruction3D;

/**
 * @brief The ActivePlan class
 * This class represent the current active plan
 */
class ActivePlan
{
public:
    ActivePlan(Vertex* planVertex, int planSize, Reconstruction3D* reconstruction3d);

    /**
     * @brief computeIntersections
     * Compute the intersection between two neighbor edges and another edge (using their
     * associated direction plan)
     */
    void computeIntersections();

    /**
     * @brief addEdgeDirectionEvent
     * Add the edge direction event into the priority queue (when the profiles change)
     */
    void addEdgeDirectionEvent();

    /**
     * @brief updateAtCurrentHeight
     * Update the active plan such that every vertices has the same (current) height
     * @param currentHeight
     */
    void updateAtCurrentHeight(float currentHeight);

    /**
     * @brief insert2Edges
     * When an edge is splitted into to edge, replace the old edge by the two new edges
     * @param old
     * @param new1
     * @param new2
     */
    void insert2Edges(Edge* old, Edge* new1, Edge* new2);

    void getActivePlanCopy(std::vector< std::vector< Edge* > >* copy);

    /**
     * @brief removeInvalidEdges
     * Remove all invalid edges from the current active plan
     */
    void removeInvalidEdges();

    /**
     * @brief eliminateParallelNeighbor
     * If two neighboring edges are parallel, we will not be able
     * to find an intersection. We have thus to eliminate one of the two neighbor
     */
    void eliminateParallelNeighbor();

    /**
     * @brief size
     * @return the size of the current active plan
     */
    float size();

    /**
     * @brief checkConsistency
     * Check if the active plan has not been corrupted
     */
    void checkConsistency();

private:
    // used to access all usefull variables
    Reconstruction3D* reconstruction3d;
    // the current active plan
    std::vector<Edge*> activePlan;

    /**
     * @brief isIntersectionCorrect
     * The intersection is between three unbounded plan. This method check if the intersection
     * is really between the three edges by check the bound of the plan
     * @param intersection
     * @param edge3
     * @return true if the intersection is correct
     */
    bool isIntersectionCorrect(GeneralEvent* intersection, Edge* edge3);

    /**
     * @brief isIntersectionWithChildCorrect
     * When an edge is splitted into two new edges, then the intersection
     * is with one of the two new edges
     * @param intersection
     * @param old
     * @param child1
     * @param child2
     * @return The new edge (child) with which the intersection is correct
     */
    Edge* isIntersectionWithChildCorrect(GeneralEvent* intersection, Edge* old, Edge* child1, Edge* child2);

    /**
     * @brief addNewTriangle
     * Add a new triangle to show
     * @param vertex1
     * @param vertex2
     * @param vertex3
     */
    void addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3);
};

#endif // ACTIVEPLAN_H
