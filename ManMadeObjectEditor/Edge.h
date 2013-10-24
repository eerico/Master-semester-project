#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsLineItem>
#include "Utils.h"

class Vertex;
class Profile;

class Edge
{
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

private:
    Vertex* vertex1;
    Vertex* vertex2;
    Profile* profile;
    QGraphicsLineItem* lineItem;
};

#endif // EDGE_H
