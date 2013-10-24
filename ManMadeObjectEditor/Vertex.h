#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <QGraphicsEllipseItem>
#include <QPen>
#include "Edge.h"
//class Edge;

class Vertex
{
public:
    Vertex(float x, float y);
    virtual ~Vertex();

    float getX();
    float getY();
    void setX(float i);
    void setY(float i);

    Vertex* getNeighbor1();
    Vertex* getNeighbor2();
    void setNeighbor1(Vertex *vx);
    void setNeighbor2(Vertex *vx);
    Edge* replaceNeighbour(Vertex* oldVx, Vertex* newVx);

    QGraphicsEllipseItem* getEllipse();
    void setEllipse(QGraphicsEllipseItem* e);

    Edge* getEdge1();
    Edge* getEdge2();
    void setEdge1(Edge* e);
    void setEdge2(Edge* e);

    Edge* removeVertex();


private:
    float x;
    float y;
    QGraphicsEllipseItem* ellipse;
    Vertex *neighbor1;
    Edge* edge1;
    Vertex *neighbor2;
    Edge* edge2;
};

#endif // VERTEX_H
