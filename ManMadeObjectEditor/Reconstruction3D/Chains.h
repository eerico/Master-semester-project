#ifndef CHAINS_H
#define CHAINS_H
#include <QGLViewer/qglviewer.h>
#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "../Geometry/Profile.h"
#include "ChainOrientationComparator.h"
#include "Intersection.h"
#include "ActivePlan.h"
#include <algorithm>
#include <cmath>

class Chains
{
public:
    Chains(Intersection *intersection, std::vector<qglviewer::Vec *> *triangles, ActivePlan *activePlan);
    void intraChainHandling();
    bool interChainHandling();

private:
    Intersection* intersection;
    std::vector<qglviewer::Vec *> *triangles;
    std::vector< std::vector< Edge* >* > chainList;
    ActivePlan* activePlan;
    void addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3);
    void splitEdgeAtCorner(Edge *edgeToSplit, Edge*& newEdge1, Edge*& newEdge2);
};

#endif // CHAINS_H
