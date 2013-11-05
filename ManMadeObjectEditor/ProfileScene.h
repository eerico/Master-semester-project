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
#include "Profile.h"
#include "MeshManager.h"
#include "Utils.h"
#include "Edge.h"
#include "ProfileDestructorManager.h"

class ProfileScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ProfileScene(MeshManager* meshManager);
    ~ProfileScene();

public slots:
    void newProfileSelected();

private:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void addVertex(QPoint mousePos);
    void removeVertex();
    void moveVertex();

    bool isProfileSelected;
    bool isVertexMoving;

    static const int vertexRadius;
    MeshManager* meshManager;
    Profile* currentProfile;

    void basicBezier(QPoint* mousePos, int numSample);
    void loadProfile();
    Vertex* currentlyMovingVertex;
};

#endif // PROFILESCENE_H
