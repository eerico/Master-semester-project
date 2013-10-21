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
#include "Mesh.h"
#include "Utils.h"

class ProfileScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ProfileScene(Mesh* mesh);
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

    void keyPressEvent(QKeyEvent* keyEvent);
    void keyReleaseEvent(QKeyEvent *event);

    bool ctrl_pressed;
    bool shift_pressed;
    bool isProfileSelected;
    bool isVertexMoving;

    static const int vertexRadius;
    Mesh* mesh;
    Profile* currentProfile;

    void basicBezier(QPoint* mousePos, int numSample);
    void loadProfile();
    Vertex* currentlyMovingVertex;
};

#endif // PROFILESCENE_H
