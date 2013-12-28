#ifndef SIMPLIFICATIONPROFILESCENE_H
#define SIMPLIFICATIONPROFILESCENE_H

#include "SimplificationScene.h"
#include "../MeshManager.h"
#include "../Simplification/Curve.h"
#include <vector>
#include <QGraphicsSceneMouseEvent>

/**
 * @brief The SimplificationProfileScene class
 * This scene is used during the simplification of the selected profile
 */
class SimplificationProfileScene : public SimplificationScene
{
public:
    /**
     * @brief SimplificationProfileScene
     * @param meshManager
     * @param curveArray An empty curve array that will be filled when the user select the
     * different part to simplify
     */
    SimplificationProfileScene(MeshManager* meshManager, std::vector< Curve* >* curveArray);
    ~SimplificationProfileScene();

    /**
     * @brief revertColor
     * If some changes (color, brush) has been made on the vertices and the edges
     * this method restore to their inital value
     */
    void revertColor();

private:
    /**
     * @brief loadProfile
     * Load the current selected profile in this scene
     */
    void loadProfile();

    /**
     * @brief mousePressEvent
     * The user can select which part to simplify. This is modeled by creating Curve
     * and we put each curve into the curve array
     * @param mouseEvent
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

    /**
     * @brief updateSceneEllipseAndLineColor
     * Update the currently drawn profile such that the user know which
     * curve he has already defined
     */
    void updateSceneEllipseAndLineColor();
};
#endif // SIMPLIFICATIONPROFILESCENE_H
