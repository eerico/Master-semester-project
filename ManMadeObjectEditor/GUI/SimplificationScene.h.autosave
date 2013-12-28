#ifndef SIMPLIFICATIONSCENE_H
#define SIMPLIFICATIONSCENE_H

#include "../MeshManager.h"
#include "../Simplification/Curve.h"
#include <vector>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QGraphicsScene>

/**
 * @brief The SimplificationScene class
 * Abstract class that all simplification scene must inherit
 */
class SimplificationScene : public QGraphicsScene
{
public:
    SimplificationScene(MeshManager* meshManager, std::vector< Curve* >* curveArray);
    virtual ~SimplificationScene();

    /**
     * @brief revertColor
     * If some changes (color, brush) has been made on the vertices and the edges
     * this method restore to their inital value
     */
    virtual void revertColor() = 0;

protected:
    MeshManager* meshManager;

    // The old data about pen and brush for the vertices and edges
    std::vector< QPen > oldPenEllipse;
    std::vector< QBrush > oldBrushEllipse;
    std::vector< QPen > oldPenLine;

    // The current curve that is currently constructed
    Curve* currentCurve;

    // The array containing all curve
    std::vector< Curve* >* curveArray;

    /**
     * @brief mousePressEvent
     * The user can select which part to simplify. This is modeled by creating Curve
     * and we put each curve into the curve array
     * @param mouseEvent
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) = 0;

    /**
     * @brief updateSceneEllipseAndLineColor
     * Update the currently drawn profile/floor plan such that the user know which
     * curve he has already defined
     */
    virtual void updateSceneEllipseAndLineColor() = 0;
};

#endif // SIMPLIFICATIONSCENE_H
