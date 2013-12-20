#include "Reconstruction3D.h"

//#define DEBUG

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec * > *triangles
                                   , std::vector<std::vector< std::vector< Edge* > > >* chainsDebug
                                   , std::vector<std::vector< std::vector< Edge* > > >* chainsDebug2
                                   , std::vector<std::vector<Edge *> > *activePlanDebug)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles)
    , chainsDebug(chainsDebug), chainsDebug2(chainsDebug2), activePlanDebug(activePlanDebug)
{
}

Reconstruction3D::~Reconstruction3D()
{
    GeneralDestructorManager::deleteObject();
}

void Reconstruction3D::reconstruct()
{
    // there is nothing to reconstruct
    if (floorPlanSize == 0) {
        return;
    }


}
