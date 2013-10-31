#include "Edge.h"
#include "Vertex.h"
#include "Profile.h"

Edge::Edge(Vertex* vertex1, Vertex* vertex2, Profile* p)
    :vertex1(vertex1), vertex2(vertex2), profile(p), lineItem(0), normal(0)
{
    edgePen.setWidth(3);
}

Edge::~Edge()
{
	if (normal != 0) {
		delete normal;
	}
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

QGraphicsLineItem* Edge::computeLineItem()
{
    lineItem = new QGraphicsLineItem(vertex1->getEllipse()->rect().center().rx(),
                                     vertex1->getEllipse()->rect().center().ry(),
                                     vertex2->getEllipse()->rect().center().rx(),
                                     vertex2->getEllipse()->rect().center().ry());


    if(profile != 0) {
        edgePen.setColor(*profile->getProfileColorIdentification());
    }
    lineItem->setPen(edgePen);
    lineItem->setZValue(Utils::getZValueBackground());

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
    Utils::normalize(comparedEdgeDirectionX, comparedEdgeDirectionY);

    float dotProduct = Utils::dotProduct(thisEdgeDirectionX, thisEdgeDirectionY, comparedEdgeDirectionX, comparedEdgeDirectionY);

    // what error can we consider maximal
    //before considering the two edge not parallel ?
    return (std::abs(1.0f - dotProduct) < 0.01f) || (std::abs(-1.0f - dotProduct) < 0.01f);
}

// the two edges must be parallel to use this method
float Edge::distance(Edge* edge)
{
    return distance(edge->getVertex2());
}

float Edge::distance(Vertex* vertex)
{
    float fromX = this->getVertex1()->getX();
    float fromY = this->getVertex1()->getY();
    float toX = this->getVertex2()->getX();
    float toY = this->getVertex2()->getY();

    float vectorEdgeX = toX - fromX;
    float vectorEdgeY = toY - fromY;

    Utils::normalize(vectorEdgeX, vectorEdgeY);

    float toXVertex = vertex->getX();
    float toYVertex = vertex->getY();

    float vectorVertexEdgeX = toXVertex - fromX;
    float vectorVertexEdgeY = toYVertex - fromY;

    Utils::normalize(vectorVertexEdgeX, vectorVertexEdgeY);

    //if the two vectors are identical, the distance is 0
    if ((std::abs(vectorEdgeX - vectorVertexEdgeX) < 0.001f) && (std::abs(vectorEdgeY - vectorVertexEdgeY) < 0.001f)) {
        return 0.0f;
    }

    // ||a||*||b||*cos, a = b = 1 because we normalized each vector before
    float dotProduct = Utils::dotProduct(vectorEdgeX, vectorEdgeY, vectorVertexEdgeX, vectorVertexEdgeY);

    float angle = std::acos(dotProduct);
    float hypotenuse = Utils::distance(fromX, fromY, toXVertex, toYVertex);
    float sinTimesHypotenuse = std::sin(angle) * hypotenuse;

    return sinTimesHypotenuse;
}


void Edge::setNormal(OMMesh::Normal *n)
{
    normal = n;
}

OpenMesh::PolyMesh_ArrayKernelT<>::Normal* Edge::getNormal()
{
    return normal;
}
