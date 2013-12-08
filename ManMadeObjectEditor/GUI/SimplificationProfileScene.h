#ifndef SIMPLIFICATIONPROFILESCENE_H
#define SIMPLIFICATIONPROFILESCENE_H

#include "SimplificationScene.h"
#include "../MeshManager.h"
#include "../Simplification/Curve.h"
#include <vector>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

class SimplificationProfileScene : public SimplificationScene
{
public:
    SimplificationProfileScene(MeshManager* meshManager, std::vector< Curve* >* curveArray);
    ~SimplificationProfileScene();

    void revertColor();

private:
    void loadProfile();
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void updateSceneEllipseAndLineColor();
};
#endif // SIMPLIFICATIONPROFILESCENE_H
