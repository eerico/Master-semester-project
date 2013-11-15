#ifndef CHAIN_H
#define CHAIN_H

#include "Intersection.h"
#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include <QGLViewer/qglviewer.h>

class Chain
{
public:
    Chain(Intersection intersection, std::vector< Edge* >* activePlan, std::vector< qglviewer::Vec * >* triangles);
    void intraChainHandling();
    void interChainHandling();

private:
    void splitEdgeAtCorner(Edge *edgeToSplit, Edge*& newEdge1, Edge*& newEdge2);
    void addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3);

    std::vector< std::vector< Edge* >* > chains;
    Intersection intersection;
    std::vector< Edge* >* activePlan;
    std::vector< qglviewer::Vec * >* triangles;
};

#endif // CHAIN_H
