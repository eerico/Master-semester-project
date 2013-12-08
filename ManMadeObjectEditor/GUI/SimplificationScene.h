#ifndef SIMPLIFICATIONSCENE_H
#define SIMPLIFICATIONSCENE_H

#include "../MeshManager.h"
#include "../Simplification/Curve.h"
#include <vector>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QGraphicsScene>

class SimplificationScene : public QGraphicsScene
{
public:
    SimplificationScene(MeshManager* meshManager, std::vector< Curve* >* curveArray);
    virtual ~SimplificationScene();
    virtual void revertColor() = 0;

protected:
    MeshManager* meshManager;

    std::vector< QPen > oldPenEllipse;
    std::vector< QBrush > oldBrushEllipse;
    std::vector< QPen > oldPenLine;

    Curve* currentCurve;

    std::vector< Curve* >* curveArray;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) = 0;
    virtual void updateSceneEllipseAndLineColor() = 0;
};

#endif // SIMPLIFICATIONSCENE_H
