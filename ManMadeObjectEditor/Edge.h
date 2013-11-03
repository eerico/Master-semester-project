#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsLineItem>
#include "Utils.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <QPen>
#include <QBrush>

class Vertex;
class Profile;

class Edge
{
	typedef OpenMesh::PolyMesh_ArrayKernelT<>  OMMesh;
public:
    Edge(Vertex* vertex1, Vertex* vertex2, Profile* p = 0);
    ~Edge();

    Profile* getProfile();
    void setProfile(Profile* p);

    Vertex* getVertex1();
    void setVertex1(Vertex* vertex);

    Vertex* getVertex2();
    void setVertex2(Vertex* vertex);

    void setLineItem(QGraphicsLineItem* item);
    QGraphicsLineItem* getLineItem();
    QGraphicsLineItem* computeLineItem();

    bool isParallel(Edge* edge);
	float distance(Edge* edge);
    float distance(Vertex* vertex);

    void setNormal(OMMesh::Normal *n);
    OMMesh::Normal* getNormal();

private:
    Vertex* vertex1;
    Vertex* vertex2;
    Profile* profile;
    QGraphicsLineItem* lineItem;
    OMMesh::Normal *normal;

    QPen edgePen;

    bool valid;
    bool isValid();
    void setValid(bool valid);
    void invalid();
};

#endif // EDGE_H
