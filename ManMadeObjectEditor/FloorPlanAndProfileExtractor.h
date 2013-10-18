#ifndef FLOORPLANANDPROFILEEXTRACTOR_H
#define FLOORPLANANDPROFILEEXTRACTOR_H

#include "FloorVertex.h"
#include "ProfileDestructorManager.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

class FloorPlanAndProfileExtractor
{
    typedef OpenMesh::PolyMesh_ArrayKernelT<>  OMMesh;

    struct TmpFloorVertex {
        OMMesh::Point point;
        OMMesh::Point neighbor1;
        OMMesh::Point neighbor2;
        bool neighbor1Used;
        bool neighbor2Used;
    };

public:
    FloorPlanAndProfileExtractor();
    void extract(OMMesh* inputMesh, FloorVertex*& floorPlan, Profile*& currentProfile, unsigned int& floorPlanSize);

private:
    float distance(float x1, float y1, float x2, float y2);
    void inversePolar(float x, float y, float& r, float& phi);
    void extractAllPlan(OMMesh* inputMesh);
    void floorPlanConstruction(FloorVertex*& floorPlan, unsigned int& floorPlanSize);
    void profileConstruction(OMMesh* inputMesh, FloorVertex*& floorPlan,
                             Profile*& currentProfile, unsigned int& floorPlanSize);

    // array of plan
    std::vector<std::vector<OMMesh::VertexIter>* > plan;
    //tmp variable used to construct the floor plan
    std::vector<TmpFloorVertex> tmpFloorPlanVector;

    //TODO trouver comment computer ces chiffre magique
    // la c est exemple avec le cone
    float dz; //le delta entre 2 plan
    int totalNumPlan; // nombre de plan jusqu au sommet
    float baseZ; // le z minimum ou se trouve le floor plan

    // pas le vrai floorplan, un cran au dessus afin d eviter d'avoir un fond plein donc que 2 neighbor au meme
    // plan par vertex!
    int floorPlanIndex;

};

#endif // FLOORPLANANDPROFILEEXTRACTOR_H
