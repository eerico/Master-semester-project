#ifndef FLOORPLANANDPROFILEEXTRACTOR_H
#define FLOORPLANANDPROFILEEXTRACTOR_H


#include "Geometry/Vertex.h"
#include "Geometry/Profile.h"
#include "ProfileDestructorManager.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

/**
 * @brief The FloorPlanAndProfileExtractor class
 * Given a mesh loaded with OpenMesh, this class extract all floor plan and create the profiles for
 * each edges
 */
class FloorPlanAndProfileExtractor
{
    // define the OpenMesh type
    typedef OpenMesh::PolyMesh_ArrayKernelT<>  OMMesh;
public:
    FloorPlanAndProfileExtractor();

    /**
     * @brief extract
     * This method will extract and construct every floor plan and every profiles given a mesh
     * with OpenMesh
     * @param inputMesh The input mesh loaded with OpenMesh
     * @param floorPlan The extracted floor plan (linked list of vertices)
     * @param currentProfile The profile that the user can modify
     * @param floorPlanSize The number of vertices in the linked list of vertices
     * @param plansAbove All plan above the first floor plan
     * @return True if the extraction do not failed
     */
    bool extract(const OMMesh* inputMesh, Vertex*& floorPlan, Profile*& currentProfile, unsigned int& floorPlanSize,
                 std::vector< std::vector< Vertex* > >& plansAbove);
    
private:
    /**
     * @brief profileDecimation
     * Each edge has a profile object, but two profile can be the same even if it is different object.
     * Thus this method ensure that if two or more edges have the same profile, then this is the same profile object.
     * @param firstFloorPlanlevel The floor plan at the first level
     */
    void profileDecimation(std::vector<Vertex*>& firstFloorPlanlevel);

    /**
     * @brief profileConstruction
     * Construct the profile for every edges
     * @param inputMesh the mesh load with OpenMesh
     * @param plans A vector that contain all floor plan at every level
     */
    void profileConstruction(const OMMesh* inputMesh, std::vector< std::vector< Vertex* > >& plans);

    /**
     * @brief recenter
     * This method will move every vertices such that (0, 0) is at the center
     * of the first floor plan
     * @param plans A vector that contain all floor plan at every level
     */
    void recenter(std::vector< std::vector< Vertex* > >& plans);

    /**
     * @brief rescale
     * This method scale all floor plan at every level such that the first floor plan is between [-1, 1]x[-1, 1]
     * @param plans A vector that contain all floor plan at every level
     */
    void rescale(std::vector< std::vector< Vertex* > >& plans);

    /**
     * @brief findMinMaxYValueMesh
     * Find the maximum and minimum height of the input mesh
     * @param inputMesh
     * @param minY
     * @param maxY
     */
    void findMinMaxYValueMesh(const OMMesh* inputMesh, float &minY, float &maxY);

    /**
     * @brief extractAllPlans
     * Given a mesh with OpenMesh, this method extract all plan at every level and put the result
     * into the vector plans
     * @param plans  A vector that will contain all floor plan at every level if the extraction succeeded
     * @param inputMesh
     * @return True if the extration succeeded
     */
    bool extractAllPlans(std::vector< std::vector<Vertex*> > &plans, const OMMesh* inputMesh);

    /**
     * @brief upsideDownCorrection
     * If the first floor plan doesnt have at least 3 vertices, then maybe the mesh was upside down.
     * We can then try to correct it
     * @param plans A vector that contain all floor plan at every level
     */
    void upsideDownCorrection(std::vector< std::vector<Vertex*> > &plans);

    // the delta height between different floor plan (the distance between each level)
    float dy;

    // the height of the first floor plan
    float baseY;

    // the number of levels that we want to extract (number of different floor plan)
    int levels;

    // variable used to control the scaling in the rescale method
    // it can be used to avoid the first floor plan to be exactly between [-1, 1]x[-1, 1]
    float border;
};

#endif // FLOORPLANANDPROFILEEXTRACTOR_H
