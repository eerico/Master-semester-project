#include "Vertex.h"
/*
Vertex::Vertex(float x, float y, FloorEdge* edge)
    : x(x), y(y), profile(profile)
{
    edge->splitEdgeAtVertex(this, edge1, edge2);
    delete edge;
}*/


Vertex::Vertex(float x, float y)
    : x(x), y(y), neighbor1(0), neighbor2(0), edge1(0), edge2(0), ellipse(0)
{
    edgePen.setWidth(2);
}

Vertex::~Vertex()
{

}

QGraphicsEllipseItem* Vertex::getEllipse(){
    return ellipse;
}

void Vertex::setNeighbor1(Vertex *vx)
{
    neighbor1 = vx;
}

void Vertex::setNeighbor2(Vertex *vx)
{
    neighbor2 = vx;
}

QGraphicsLineItem* Vertex::replaceNeighbour(Vertex* oldVx, Vertex* newVx){
    QGraphicsLineItem* newEdge(0);

    if(oldVx == neighbor1){

        neighbor1 = newVx;
        edge1 = new QGraphicsLineItem(ellipse->rect().center().rx(),
                        ellipse->rect().center().ry(),
                        neighbor1->getEllipse()->rect().center().rx(),
                        neighbor1->getEllipse()->rect().center().ry());
        edge1->setPen(edgePen);
        newEdge = edge1;

    } else if (oldVx == neighbor2){

        neighbor2 = newVx;
        edge2 = new QGraphicsLineItem(ellipse->rect().center().rx(),
                    ellipse->rect().center().ry(),
                    neighbor2->getEllipse()->rect().center().rx(),
                    neighbor2->getEllipse()->rect().center().ry());
        edge2->setPen(edgePen);
        newEdge = edge2;
    } else {
        std::cerr << "Something wrong! tried to add a point between 2 points that are not neighbour" <<
std::endl;
    }
    return newEdge;
}

QGraphicsLineItem* Vertex::removeVertex() {
    QGraphicsLineItem* edge = 0;

    if (neighbor1 != 0 && neighbor2 != 0) {
        neighbor1->setNeighbor2(neighbor2);
        neighbor2->setNeighbor1(neighbor1);

        edge = new QGraphicsLineItem(neighbor1->getEllipse()->rect().center().rx(),
                                                   neighbor1->getEllipse()->rect().center().ry(),
                                                   neighbor2->getEllipse()->rect().center().rx(),                                                   neighbor2->getEllipse()->rect().center().ry());
        neighbor1->setEdge2(edge);
        neighbor2->setEdge1(edge);
        edge->setPen(edgePen);
    } else if (neighbor1 != 0 && neighbor2 == 0) {
        neighbor1->setNeighbor2(neighbor2);
        neighbor1->setEdge2(edge);
    } else if (neighbor2 != 0 && neighbor1 == 0){
        neighbor2->setNeighbor1(neighbor1);
        neighbor2->setEdge1(edge);
    }
    return edge;
}

float Vertex::getX()
{
    return x;
}

float Vertex::getY()
{
    return y;
}

Vertex* Vertex::getNeighbor1()
{
    return neighbor1;
}

Vertex* Vertex::getNeighbor2()
{
    return neighbor2;
}

bool Vertex::addEdge1(){
    bool added = false;
    if(edge1 == 0){
        edge1 = new QGraphicsLineItem(ellipse->rect().center().rx(), ellipse->rect().center().ry(), neighbor1->getEllipse()->rect().center().rx(), neighbor1->getEllipse()->rect().center().ry());
        edge1->setPen(edgePen);
        added = true;
        if(this == neighbor1->getNeighbor1()){
            neighbor1->setEdge1(edge1);
        }else if (this == neighbor1->getNeighbor2()){
            neighbor1->setEdge2(edge1);
        }else {
            std::cerr << "Something went wrong! we have a neigbour that thinks he is not a neighbour." <<
std::endl;
            added= false;
        }
    }
    return added;
}

bool Vertex::addEdge2(){
    bool added = false;
    if(edge2 == 0){
        edge2 = new QGraphicsLineItem(ellipse->rect().center().rx(), ellipse->rect().center().ry(), neighbor2->getEllipse()->rect().center().rx(), neighbor2->getEllipse()->rect().center().ry());
        edge2->setPen(edgePen);
        added = true;
        if(this == neighbor2->getNeighbor1()){
            neighbor2->setEdge1(edge2);
        }else if (this == neighbor2->getNeighbor2()){
            neighbor2->setEdge2(edge2);
        }else {
            std::cerr << "Something went wrong! we have a neigbour that thinks he is not a neighbour." <<
std::endl;
            added= false;
        }
    }
    return added;
}

QGraphicsLineItem* Vertex::getEdge1(){
    return edge1;
}

QGraphicsLineItem* Vertex::getEdge2(){
    return edge2;
}

void Vertex::setEdge1(QGraphicsLineItem* e){
    edge1 = e;
}

void Vertex::setEdge2(QGraphicsLineItem* e){
    edge2 = e;
}

void Vertex::setEllipse(QGraphicsEllipseItem * e){
    ellipse = e;
}

void Vertex::setY(float i){
    y = i;
}

void Vertex::setX(float i){
    x = i;
}

/*
Vertex::~Vertex()
{
    delete edge1;
    delete edge2;
}
*/
