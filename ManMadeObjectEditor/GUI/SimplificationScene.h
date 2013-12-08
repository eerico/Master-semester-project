#ifndef SIMPLIFICATIONSCENE_H
#define SIMPLIFICATIONSCENE_H

#include <QGraphicsScene>
#include "../MeshManager.h"
#include "../Simplification/Curve.h"
#include <vector>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

class SimplificationScene : public QGraphicsScene
{
public:
    SimplificationScene(MeshManager* meshManager, std::vector< Curve* >* curveArray);
    ~SimplificationScene();

    void revertColor();

private:
    void loadFloorPlan();

    MeshManager* meshManager;

    std::vector< QPen > oldPenEllipse;
    std::vector< QBrush > oldBrushEllipse;
    std::vector< QPen > oldPenLine;

    Curve* currentCurve;

    std::vector< Curve* >* curveArray;

    // used to allow the user to make a closed curve
    Vertex* firstVertexSelected;

    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void updateSceneEllipseAndLineColor();

};

#endif // SIMPLIFICATIONSCENE_H
