#ifndef FLOORPLANANDPROFILEEXTRACTOR_H
#define FLOORPLANANDPROFILEEXTRACTOR_H


#include "Vertex.h"
#include "Profile.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

class FloorPlanAndProfileExtractor
{
    typedef OpenMesh::PolyMesh_ArrayKernelT<>  OMMesh;


public:
    FloorPlanAndProfileExtractor();
    void extract(OMMesh* inputMesh, Vertex*& floorPlan, Profile*& currentProfile, unsigned int& floorPlanSize);
};

#endif // FLOORPLANANDPROFILEEXTRACTOR_H
