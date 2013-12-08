#ifndef PROFILESCENE_H
#define PROFILESCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QRectF>
#include <QPoint>
#include <iostream>
#include <vector>
#include <QGraphicsEllipseItem>
#include <QList>
#include "../Geometry/Profile.h"
#include "../MeshManager.h"
#include "../Utils.h"
#include "../Geometry/Edge.h"
#include "../GarbageCollector/ProfileDestructorManager.h"

/**
 * @brief The ProfileScene class
 * This class is a scene used to show the selected profile and allow interaction
 * with this profile
 */
class ProfileScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ProfileScene(MeshManager* meshManager);
    ~ProfileScene();

public slots:

    /**
     * @brief newProfileSelected
     * Used to inform this scene that a user has selected a new profile
     */
    void newProfileSelected();

private:

    /**
     * @brief mousePressEvent
     * Allow the user to add, remove or move vertices on the profile
     * @param mouseEvent
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

    /**
     * @brief mouseReleaseEvent
     * Used to know when the user has finished to move a vertex
     * @param event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    /**
     * @brief mouseMoveEvent
     * Used when the user move a vertex. It will update its coordinates.
     * @param event
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /**
     * @brief addVertex
     * Add a vertex on the profile, either on an edge, or at the end of the profile
     * @param mousePos
     */
    void addVertex(QPoint mousePos);

    /**
     * @brief removeVertex
     * Remove a vertex if it is not the first one
     */
    void removeVertex();

    /**
     * @brief moveVertex
     * Remove the vertex that is under the mouse.
     */
    void moveVertex();

    /**
     * @brief loadProfile
     * Load and draw the profile that the user has selected
     */
    void loadProfile();

    // boolean used to know if a profile is selected or not
    bool isProfileSelected;

    // Used to know if a vertex is currently being moved with the mouse, and thus using action
    // defined in the mouse move event
    bool isVertexMoving;

    // the radius of the vertex that will be drawn
    static const int vertexRadius;

    // All information about the floor plan and profile are inside this object
    MeshManager* meshManager;

    // the profile that is currently represented in the scene
    Profile* currentProfile;

    // The vertex that is currently being moved
    Vertex* currentlyMovingVertex;
};

#endif // PROFILESCENE_H
