#ifndef CHAIN_H
#define CHAIN_H

#include "Intersection.h"
#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "Plan.h"
#include <QGLViewer/qglviewer.h>

class Chain
{
public:
    Chain(float height, Chain* previousChains, std::vector< qglviewer::Vec * >* triangles);
    Chain(Vertex* floorPlan, unsigned int floorPlanSize);
    void intraChainHandling();
    void interChainHandling();
    std::vector<std::vector<Edge *>* >* getChains();
    void printChain();
    bool isEmpty();
    bool hasChanged();
    void computeTriangle();

private:
    void addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3);
    void createTwoChain(Edge* edgeToSplit, Vertex* corner);

    std::vector< std::vector< Edge* >* >* chains;
    Chain* previousChains;
    std::vector< qglviewer::Vec * >* triangles;
    bool chainHasChanged;

    static const float collapseThreshold;
    static const float splitThreshold;
    static const float maxSize;
};

#endif // CHAIN_H
