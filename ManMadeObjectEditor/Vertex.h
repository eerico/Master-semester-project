#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include "Edge.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

//class Edge;

class Vertex
{
    typedef OpenMesh::PolyMesh_ArrayKernelT<>  OMMesh;

public:
    Vertex(float x, float y, bool addFaces=false, bool valid = false);
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

    std::vector<OMMesh::FaceHandle>* getFaces();
    void addFace(OMMesh::FaceHandle face);

    bool isValid();
    void setValid(bool valid);
    void invalid();

    Edge* removeVertex();

private:
    float x;
    float y;
    QGraphicsEllipseItem* ellipse;
    Vertex *neighbor1;
    Edge* edge1;
    Vertex *neighbor2;
    Edge* edge2;
    bool bFaces;
    bool valid;
    std::vector<OMMesh::FaceHandle>* faces;

    QPen vertexPen;
    QBrush vertexBrush;
};

#endif // VERTEX_H
