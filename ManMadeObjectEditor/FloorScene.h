#ifndef FLOORSCENE_H
#define FLOORSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QRectF>
#include <QPoint>
#include <iostream>
#include <vector>
#include <QGraphicsEllipseItem>
#include <QColor>
#include "Vertex.h"
#include "MeshManager.h"
#include "ProfileDestructorManager.h"
#include "Utils.h"
#include "Edge.h"

/**
 * @brief The FloorScene class
 * This class is a scene used to show the first floor plan and allow interaction
 * with this floor plan
 */
class FloorScene : public QGraphicsScene
{
    Q_OBJECT
public:
    FloorScene(MeshManager* meshManager);
    ~FloorScene();

signals:
    /**
     * @brief newProfileSelected
     * Used to advertise other class that a new profile is selected
     */
    void newProfileSelected();

public slots:
    /**
     * @brief loadFloorPlan
     * Load and draw the first floor plan defined in the mesh manager
     */
    void loadFloorPlan();

private:
    /**
     * @brief mousePressEvent
     * Allow the user to add, remove or move vertices on the floor plan
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
     * Add a vertex if the user has clicked on an edge
     * @param mousePos The mouse position
     */
    void addVertex(QPoint mousePos);

    /**
     * @brief removeVertex
     * Remove the vertex that is under the mouse.
     */
    void removeVertex();

    /**
     * @brief moveVertexOrLoadProfile
     * Action called when the user use a simple click on an edge or on a vertex.
     * If he has clicked on an edge, then the profile is loaded. If he has clicked
     * on a vertex, then the vertex will be moved
     */
    void moveVertexOrLoadProfile();

    /**
     * @brief basicCircle
     * Create a new floor plan by taking uniform sample (where the number is defnied by
     * numSample) on a circle. If the floor plan is empty, then when the user click on the
     * floor plan scene, this method will be called
     * @param mousePos The mouse position
     * @param numSample The number of sample we will take
     */
    void basicCircle(QPoint* mousePos, int numSample);

    /**
     * @brief newProfileSelected
     * Tells the mesh manager and other classes that a new profile has been selected
     * @param p The new selected profile
     */
    void newProfileSelected(Profile* p);

    // Used to know if a vertex is currently being moved with the mouse, and thus using action
    // defined in the mouse move event
    bool isVertexMoving;

    // All information about the floor plan and profile are inside this object
    MeshManager* meshManager;

    // The vertex that is currently being moved
    Vertex* currentlyMovingVertex;

    // the radius of the vertex that will be drawn
    static const int vertexRadius;
};

#endif // FLOORSCENE_H
