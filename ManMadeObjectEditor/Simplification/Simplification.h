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

    std::vector<Vertex*>* simplifyFloorPlanPreview();
    std::vector<Vertex*>* simplifyProfilePreview();

private:
    std::vector< Curve* >* curveArray;
    float threshold;
    MeshManager* meshManager;

    void simplifyFloorPlan(Curve* curve, bool forPreview);
    void simplifyProfile(Curve* curve, bool forPreview);

    void updateCurveArray(std::vector<Curve *> *curveArrayUpdated, Vertex *original, Vertex *clone);
};

#endif // SIMPLIFICATION_H
