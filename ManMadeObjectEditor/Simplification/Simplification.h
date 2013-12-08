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

    void simplifyFloorPlan();
    void simplifyProfile();

private:
    std::vector< Curve* >* curveArray;
    float threshold;
    MeshManager* meshManager;

    void simplifyFloorPlan(Curve* curve);
    void simplifyProfile(Curve* curve);
};

#endif // SIMPLIFICATION_H
