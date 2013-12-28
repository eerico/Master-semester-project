#ifndef RECONSTRUCTION3D_H
#define RECONSTRUCTION3D_H

#include "../Geometry/Vertex.h"
#include "../Geometry/Edge.h"
#include "../Geometry/Profile.h"
#include <vector>
#include <QGLViewer/qglviewer.h>
#include <queue>
#include "../GarbageCollector/GeneralDestructorManager.h"
#include "Event.h"
#include "EventComparator.h"
#include "ActivePlan.h"

/**
 * @brief The Reconstruction3D class
 * This class apply the 3D reconstruction algorithm using the floor plan and the profiles.
 * It will fill the triangles vector with the triangles to show and other usefull information such
 * as the differents chains and the different active plan
 */
class Reconstruction3D
{
public:
    /**
     * @brief Reconstruction3D
     * @param floorPlan
     * @param floorPlanSize
     * @param triangles A vector where all created triangles wil be stored
     * @param chainsDebug A vector where all chains before the chain algorithm will be stored
     * @param chainsDebug2 A vector where all chains after the chain algorithm will be stored
     * @param activePlanDebug A vector where all activeplan will be stored
     */
    Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector< qglviewer::Vec * >* triangles
                     , std::vector<std::vector<std::vector<Edge *> > > *chainsDebug
                     , std::vector<std::vector<std::vector<Edge *> > > *chainsDebug2
                     , std::vector<std::vector<Edge *> >*activePlanDebug);

    ~Reconstruction3D();

    /**
     * @brief reconstruct
     * Apply the 3D reconstruction algorithm
     */
    void reconstruct();

    Vertex* floorPlan;
    unsigned int floorPlanSize;

    // a vector that contain all computed triangles
    std::vector< qglviewer::Vec * >* triangles;

    // a vector that contain all vector that will be returned after the 3D reconstruction
    std::vector< qglviewer::Vec * >* trianglesToShow;

    // the priority queue that contains all events used for the 3D reconstruction
    std::priority_queue<Event*, std::vector<Event*>, EventComparator>* priorityQueue;

    // the current active plan
    ActivePlan* activePlan;

    // the delta height used for clustering
    static const float deltaHeight;

    // the current height of the reconstruction
    float currentHeight;

    // debug information
    std::vector< std::vector< std::vector< Edge* > > >* chainsDebug;
    std::vector< std::vector< std::vector< Edge* > > >* chainsDebug2;
    std::vector< std::vector< Edge* > >* activePlanDebug;

    // variable used to stop the 3D reconstruction in case of errors
    bool* error;

private:
    /**
     * @brief errorHandling
     * clean the data if an error has occured during the 3D reconstruction algorithm
     */
    void errorHandling();
};

#endif // RECONSTRUCTION3D_H
