#ifndef SIMPLIFICATION_H
#define SIMPLIFICATION_H

#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "Curve.h"
#include "../MeshManager.h"
#include <vector>

/**
 * @brief The Simplification class
 * Simplification algorithm based on the Ramer–Douglas–Peucker algorithm
 */
class Simplification
{
public:
    Simplification(std::vector< Curve* >* curveArray, MeshManager* meshManager, float threshold);

    /**
     * @brief simplifyFloorPlan
     * simplifiy the floor plan based on the curve array
     */
    void simplifyFloorPlan();

    /**
     * @brief simplifyProfile
     * simplify the selected profile, based on the curve array
     */
    void simplifyProfile();

    /**
     * @brief simplifyFloorPlanPreview
     * @return a preview of the simplification
     */
    std::vector<Vertex*>* simplifyFloorPlanPreview();

    /**
     * @brief simplifyProfilePreview
     * @return a preview of the simplification
     */
    std::vector<Vertex*>* simplifyProfilePreview();

private:
    // an array of curve defining different part to simplify
    std::vector< Curve* >* curveArray;

    // the threshold used during the simplification
    float threshold;

    // the meshManager used to extract different information
    MeshManager* meshManager;

    /**
     * @brief simplifyFloorPlan
     * recursively simplify the floor plan
     * @param curve
     * @param forPreview if the simplification is done for a preview or not
     */
    void simplifyFloorPlan(Curve* curve, bool forPreview);

    /**
     * @brief simplifyProfile
     * recursively simplify the profile
     * @param curve
     * @param forPreview if the simplification is done for a preview or not
     */
    void simplifyProfile(Curve* curve, bool forPreview);

    /**
     * @brief updateCurveArray
     * replace every vertex "begin" and "end" of every curve that is equal
     * to "original" in the curve array by the clone
     * @param curveArrayUpdated
     * @param original
     * @param clone
     */
    void updateCurveArray(std::vector<Curve *> *curveArrayUpdated, Vertex *original, Vertex *clone);
};

#endif // SIMPLIFICATION_H
