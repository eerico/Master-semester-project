#ifndef CHAINS_H
#define CHAINS_H
#include <QGLViewer/qglviewer.h>
#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "../Geometry/Profile.h"
#include "ChainOrientationComparator.h"
#include "Intersection.h"
#include <algorithm>
#include <cmath>

class Chains
{
public:
    Chains(Intersection *intersection, std::vector<qglviewer::Vec *> *triangles);
    void intraChainHandling();
    void interChainHandling();

private:
    Intersection* intersection;
    std::vector<qglviewer::Vec *> *triangles;
    std::vector< std::vector< Edge* >* > chainList;
    void addNewTriangle(Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) ;
};

#endif // CHAINS_H
