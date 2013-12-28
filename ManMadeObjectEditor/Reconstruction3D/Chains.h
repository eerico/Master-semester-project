#ifndef CHAINS_H
#define CHAINS_H

#include <QGLViewer/qglviewer.h>
#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "../Geometry/Profile.h"
#include "ChainOrientationComparator.h"
#include "ActivePlan.h"
#include <algorithm>
#include <cmath>
#include <set>

class GeneralEvent;

/**
 * @brief The Chains class
 * This class create the chains and apply the two different chains algorithm
 * (intra and inter chain algorithm) on the chains.
 *
 * A chain is a list of consecutive edges
 */
class Chains
{
public:
    Chains(GeneralEvent *intersection, std::vector<qglviewer::Vec *> *triangles, ActivePlan *activePlan);
    ~Chains();

    /**
     * @brief intraChainHandling
     * Apply the intra chains algorithm. The algorithm check which edges shrink to 0 and thus become a vertex.
     * For each chain, all interior edge (all edge not in the border of the chain) shrink to 0 and they are replaced
     * by a vertex.
     */
    void intraChainHandling();

    /**
     * @brief interChainHandling
     * Apply the inter chain algorithm. The algorithm handle the case when the active plan is splitted into
     * different part. The algorithm ensure the consistency of the active plan by correctly
     * splitting it.
     * @return true if an edge has been splitted
     */
    bool interChainHandling();

    void getChains(std::vector<std::vector<std::vector<Edge *> > > *chainsAllLevel);

private:
    // The intersection used to construct the chains
    GeneralEvent* intersection;

    // the array of triangles
    std::vector<qglviewer::Vec *> *triangles;

    // the list that contains all chain for this intersection
    std::vector< std::vector< Edge* >* > chainList;

    // the associated active plan
    ActivePlan* activePlan;

    /**
     * @brief addNewTriangle
     * Add a new triangle defined by the three vertices
     * @param vertex1
     * @param vertex2
     * @param vertex3
     */
    void addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3);

    /**
     * @brief splitEdgeAtCorner
     * Split the edge (edgeToSplit) and return the two new edge (newEdge1 and newEdge2)
     * @param edgeToSplit
     * @param newEdge1
     * @param newEdge2
     */
    void splitEdgeAtCorner(Edge *edgeToSplit, Edge*& newEdge1, Edge*& newEdge2);
};

#endif // CHAINS_H
