#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include <QGLViewer/qglviewer.h>
#include <vector>
#include <iostream>
#include <QObject>
#include <cmath>

#include "Geometry/Vertex.h"
#include "FloorPlanAndProfileExtractor.h"
#include "Utils.h"
#include "ProfileDestructorManager.h"
#include "Reconstruction3D/Reconstruction3D.h"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

/**
 * @brief The MeshManager class
 * This class is used to do store and manage all information related to the mesh
 * and all floor plan.
 */
class MeshManager : public QObject
{
    Q_OBJECT

    // define the OpenMesh type
    typedef OpenMesh::PolyMesh_ArrayKernelT<>  OMMesh;
public:
    MeshManager();
    ~MeshManager();

    void setCurrentProfile(Profile* p);
    Profile* getCurrentProfile();

    unsigned int getFloorPlanSize();
    void incrementFloorPlanSize();
    void decrementFloorPlanSize();

    Vertex* getFloorPlan();
    // The floor plan is a linked list of vertices, thus only one vertex is enough to describe it
    void setFloorPlan(Vertex* vertex);

    /**
     * @brief loadMesh
     * This method will load the mesh using OpenMesh and extract all floor plan and profile and
     * signal that a new floor plan has been loaded
     * @param fileName The file containing the mesh that we want to load
     */
    void loadMesh(QString fileName);

    /**
     * @brief setUpdateOnMesh
     * Tells the mesh manager that the floor plan or profile has been modified.
     */
    void setUpdateOnMesh();

    /**
     * @brief setLongUpdateOnMesh
     * Tells the mesh manager that the floor plan or profile is currently modified
     * @param b Must be true when there is modification and false when its over
     */
    void setLongUpdateOnMesh(bool b);

    /**
     * @brief clearTriangles
     * clear the vector that contain the triangles and delete them
     */
    void clearTriangles();

    /**
     * @brief clearPoints
     * clear the vector that contain the vertices and delete them
     */
    void clearPoints();

    /**
     * @brief getTriangles
     * Do a 3D reconstruction based on the floor plan and th profiles. If there is no modification
     * on the floor plan or profiles, then this method return the old vector. Else it will recompute
     * the 3D reconstruction
     * @return A vector of triangles
     */
    const std::vector< qglviewer::Vec * >* getTriangles();

    /**
     * @brief getPoints
     * Do a 3D reconstruction based on the floor plan and th profiles. If there is no modification
     * on the floor plan or profiles, then this method return the old vector. Else it will recompute
     * the 3D reconstruction
     * @return A vector of points
     */
    const std::vector< qglviewer::Vec * >* getPoints();

    /**
     * @brief getPlans
     * @return A vector that contain all floor plan above the first one
     */
    std::vector<std::vector<Vertex * > > &getPlans();

    std::vector<std::vector<std::vector<Edge *> > > &getChains();

    std::vector<std::vector<std::vector<Edge *> > > &getChains2();

signals:
    /**
     * @brief newFloorPlan
     * Signal that a new floor plan has been loaded
     */
    void newFloorPlan();

private:

    // a vector that contain the triangles obtain with the 3D reconstruction
    std::vector< qglviewer::Vec * >* triangles;

    // a vector that contain the points obtain with the 3D reconstruction
    std::vector< qglviewer::Vec* >* points;

    // the first floor plan
    Vertex* floorPlan; // linked list, can iterate on it using the neighbor

    // the currently selected profile
    Profile* currentProfile;

    // the loaded input mesh
    OMMesh* inputMesh;

    // boolean that tells if there has been a modification since the last time we did the
    // 3D reconstruction
    bool updateOnMesh;

    // boolean that tells us that the floor plan or profile is currently modified
    bool longUpdateOnMesh;

    // the number of vertices in the first floor plan
    unsigned int floorPlanSize;

    // a vector that contain all floor plan above the first one
    std::vector< std::vector< Vertex* > > plans;

    // a vector that contain all chains for debugging
    std::vector< std::vector< std::vector< Edge* > > > chains;

    // a vector that contain all chains adter intra adn inter chain handling for debugging
    std::vector< std::vector< std::vector< Edge* > > > chains2;
};

#endif // MESH_H
