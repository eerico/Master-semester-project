#include "Edge.h"

Edge::Edge(Vertex* vertex1, Vertex* vertex2, Profile* p)
    :vertex1(vertex1), vertex2(vertex2), profile(p)
{
    lineItem = new QGraphicsLineItem(vertex1->getEllipse()->rect().center().rx(),
                                     vertex1->getEllipse()->rect().center().ry(),
                                     vertex2->getEllipse()->getEllipse()->rect().center().rx(),
                                     vertex2->getEllipse()->getEllipse()->rect().center().ry());
}

Edge::~Edge()
{
    delete lineItem;
}

Profile* Edge::getProfile()
{
    return profile;
}

void Edge::setProfile(Profile* p)
{
    profile = p;
}

Vertex* Edge::getVertex1()
{
    return vertex1;
}

void Edge::setVertex1(Vertex* vertex)
{
    vertex1 = vertex;
}

Vertex* Edge::getVertex2()
{
    return vertex2;
}

void Edge::setVertex2(Vertex* vertex)
{
    vertex2 = vertex;
}

void Edge::setLineItem(QGraphicsLineItem* item)
{
    lineItem = item;
}

QGraphicsLineItem* Edge::getLineItem()
{
    return lineItem;
}

bool Edge::isParallel(Edge* edge)
{
    //to check if the current edge and the compared edge are parallel
    //we can use the dot product between them. If the dot product
    //is 1 or -1, then they are parallel

    float fromX = vertex1->getX();
    float fromY = vertex1->getY();
    float toX = vertex2->getX();
    float toY = vertex2->getY();

    float thisEdgeDirectionX = toX - fromX;
    float thisEdgeDirectionY = toY - fromY;
    Utils::normalize(thisEdgeDirectionX, thisEdgeDirectionY);

    Vertex* comparedEdgeVertex1 = edge->getVertex1();
    Vertex* comparedEdgeVertex2 = edge->getVertex2();

    fromX = comparedEdgeVertex1->getX();
    fromY = comparedEdgeVertex1->getY();
    toX = comparedEdgeVertex2->getX();
    toY = comparedEdgeVertex2->getY();

    float comparedEdgeDirectionX = toX - fromX;
    float comparedEdgeDirectionY = toY - fromY;
    Utils::normalize(thisEdgeDirectionX, thisEdgeDirectionY);

    float dotProduct = Utils::dotProduct(thisEdgeDirectionX, thisEdgeDirectionYm, comparedEdgeDirectionX, comparedEdgeDirectionY);

    //use 0.001f for precision issues
    return (std::abs(1.0f - dotProduct) < 0.001f) || (std::abs(-1.0f - dotProduct) < 0.001f);
}
