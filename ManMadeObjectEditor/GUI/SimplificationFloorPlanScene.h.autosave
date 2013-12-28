#ifndef SIMPLIFICATIONFLOORPLANSCENE_H
#define SIMPLIFICATIONFLOORPLANSCENE_H

#include "SimplificationScene.h"
#include "../MeshManager.h"
#include "../Simplification/Curve.h"
#include <vector>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

/**
 * @brief The SimplificationFloorPlanScene class
 * This scene is used during the simplification of the floor plan
 */
class SimplificationFloorPlanScene : public SimplificationScene
{
public:
    /**
     * @brief SimplificationFloorPlanScene
     * @param meshManager
     * @param curveArray An empty curve array that will be filled when the user select the
     * different part to simplify
     */
    SimplificationFloorPlanScene(MeshManager* meshManager, std::vector< Curve* >* curveArray);
    ~SimplificationFloorPlanScene();

    /**
     * @brief revertColor
     * If some changes (color, brush) has been made on the vertices and the edges
     * this method restore to their inital value
     */
    void revertColor();

private:
    /**
     * @brief loadFloorPlan
     * Load the current floor plan in this scene
     */
    void loadFloorPlan();

    /**
     * @brief mousePressEvent
     * The user can select which part to simplify. This is modeled by creating Curve
     * and we put each curve into the curve array
     * @param mouseEvent
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

    /**
     * @brief updateSceneEllipseAndLineColor
     * Update the currently drawn floor plan such that the user know which
     * curve he has already defined
     */
    void updateSceneEllipseAndLineColor();
};

#endif // SIMPLIFICATIONFLOORPLANSCENE_H
