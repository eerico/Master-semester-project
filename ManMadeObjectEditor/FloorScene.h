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
#include <QPen>
#include <QColor>
#include "FloorVertex.h"
#include "Mesh.h"
#include "ProfileDestructorManager.h"
#include "Utils.h"

class FloorScene : public QGraphicsScene
{
    Q_OBJECT
public:
    FloorScene(Mesh* mesh);
    ~FloorScene();

signals:
    void newProfileSelected();

public slots:
    void loadFloorPlan();

private:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void addVertex(QPoint mousePos);
    void removeVertex();
    void moveVertex();

    void keyPressEvent(QKeyEvent* keyEvent);
    void keyReleaseEvent(QKeyEvent *event);

    bool ctrl_pressed;
    bool shift_pressed;
    bool isVertexMoving;

    static const int vertexRadius;
    Mesh* mesh;
    FloorVertex* currentlyMovingVertex;

    void basicCircle(QPoint* mousePos, int numSample);
    void newProfileSelected(Profile* p);


};

#endif // FLOORSCENE_H
