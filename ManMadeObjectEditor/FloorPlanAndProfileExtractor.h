#ifndef FLOORPLANANDPROFILEEXTRACTOR_H
#define FLOORPLANANDPROFILEEXTRACTOR_H


#include "Vertex.h"
#include "Profile.h"
#include "ProfileDestructorManager.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>


class FloorPlanAndProfileExtractor
{
    typedef OpenMesh::PolyMesh_ArrayKernelT<>  OMMesh;
public:
    FloorPlanAndProfileExtractor();
    void extract(const OMMesh* inputMesh, Vertex*& floorPlan, Profile*& currentProfile, unsigned int& floorPlanSize);

private:
    void profileConstruction(const OMMesh* inputMesh, std::vector< std::vector< Vertex* > >& plans);
    void recenter(std::vector< std::vector< Vertex* > >& plans);
    void rescale(std::vector< std::vector< Vertex* > >& plans);
    void findMinMaxZValueMesh(const OMMesh* inputMesh, float &minZ, float &maxZ);
    void extractAllPlans(std::vector< std::vector<Vertex*> > &plans, const OMMesh* inputMesh);
    void upsideDownCorrection(std::vector< std::vector<Vertex*> > &plans);
    float dz;
    float baseZ;
    int levels;
};

#endif // FLOORPLANANDPROFILEEXTRACTOR_H
