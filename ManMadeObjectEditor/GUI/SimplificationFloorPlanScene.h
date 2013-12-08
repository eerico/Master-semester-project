#ifndef SIMPLIFICATIONFLOORPLANSCENE_H
#define SIMPLIFICATIONFLOORPLANSCENE_H

#include "SimplificationScene.h"
#include "../MeshManager.h"
#include "../Simplification/Curve.h"
#include <vector>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

class SimplificationFloorPlanScene : public SimplificationScene
{
public:
    SimplificationFloorPlanScene(MeshManager* meshManager, std::vector< Curve* >* curveArray);
    ~SimplificationFloorPlanScene();

    void revertColor();

private:
    void loadFloorPlan();
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void updateSceneEllipseAndLineColor();

};

#endif // SIMPLIFICATIONFLOORPLANSCENE_H
