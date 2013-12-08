#ifndef SIMPLIFICATION_H
#define SIMPLIFICATION_H

#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "Curve.h"
#include "../MeshManager.h"
#include <vector>

// Simplification algorithm based on the Ramer–Douglas–Peucker algorithm

class Simplification
{
public:
    Simplification(std::vector< Curve* >* curveArray, MeshManager* meshManager, float threshold);

    void simplify();

private:
    std::vector< Curve* >* curveArray;
    float threshold;
    MeshManager* meshManager;

    void simplify(Curve* curve);
};

#endif // SIMPLIFICATION_H
