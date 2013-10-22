#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPen>


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
    QGraphicsLineItem* replaceNeighbour(Vertex* oldVx, Vertex* newVx);
    QGraphicsEllipseItem* getEllipse();
    void setEllipse(QGraphicsEllipseItem* e);
    bool addEdge1();
    bool addEdge2();
    QGraphicsLineItem* getEdge1();
    QGraphicsLineItem* getEdge2();
    void setEdge1(QGraphicsLineItem* e);
    void setEdge2(QGraphicsLineItem* e);
    void setNeighbor1(Vertex *vx);
    void setNeighbor2(Vertex *vx);
    QGraphicsLineItem* removeVertex();


private:
    float x;
    float y;
    QGraphicsEllipseItem* ellipse;
    Vertex *neighbor1;
    QGraphicsLineItem* edge1;
    Vertex *neighbor2;
    QGraphicsLineItem* edge2;
    QPen edgePen;
};

#endif // VERTEX_H
