#ifndef FLOORPLANANDPROFILEEXTRACTOR_H
#define FLOORPLANANDPROFILEEXTRACTOR_H

#include "FloorVertex.h"
#include "ProfileDestructorManager.h"
#include "Utils.h"

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
        OMMesh::VertexIter v_it;
    };

public:
    FloorPlanAndProfileExtractor();
    void extract(OMMesh* inputMesh, FloorVertex*& floorPlan, Profile*& currentProfile, unsigned int& floorPlanSize);

private:
    bool isSameAngle(float toX1, float toY1, float fromX1, float fromY1,
                    float toX2, float toY2, float fromX2, float fromY2);
    void extractAllPlan(OMMesh* inputMesh);
    void floorPlanConstruction(FloorVertex*& floorPlan, unsigned int& floorPlanSize, OMMesh* inputMesh);
    void profileConstruction(OMMesh* inputMesh, FloorVertex*& floorPlan,
                             Profile*& currentProfile, unsigned int& floorPlanSize);

    // array of plan
    std::vector<std::vector<OMMesh::VertexIter>* > plan;
    //tmp variable used to construct the floor plan
    std::vector<TmpFloorVertex> tmpFloorPlanVector;

    //TODO trouver comment computer ces chiffre magique
    // la c est exemple avec le cone
    float intervalBetweenZValue; //le delta entre 2 plan
    int totalNumPlan; // nombre de plan jusqu au sommet
    float smallestZValue; // le z minimum ou se trouve le floor plan

    // pas le vrai floorplan, un cran au dessus afin d eviter d'avoir un fond plein donc que 2 neighbor au meme
    // plan par vertex!
    int floorPlanIndex;

};

#endif // FLOORPLANANDPROFILEEXTRACTOR_H
