#include "Profile.h"
#include <iostream>

// chaque profile est diriger vers le centre de gravité du floor plan ?

// Convention: profile are directed toward the center of the floorplan
// and w max is 1.0f
Profile::Profile(bool empty): pVertex(0)
{
    if (!empty) {
        pVertex = new Vertex(0,0);
        //initProfileSkewedLine(3);
        initProfileBezier(10);
    }
}

Profile::~Profile()
{
    Vertex* next;
    while(pVertex != 0)
    {
        next = pVertex->getNeighbor2();
        delete pVertex;
        pVertex = next;
    }
}

void Profile::initProfileSkewedLine(int numSample)
{
    float fromW(0.0f);
    float fromZ(0.0f);
    float toW(1.0f);
    float toZ(1.0f);

    float w(0.0f);
    float z(0.0f);
    float increment = 1.0/(float)(numSample - 1);

    Vertex* current = pVertex;
    Vertex* previous = 0;
    float t(0.0f);

    for(int sampleCounter(0); sampleCounter < numSample; sampleCounter++) {
        w = (1.0f - t) * fromW + t * toW;
        z = (1.0f - t) * fromZ + t * toZ;
        current->setX(w);
        current->setY(z);

        current->setNeighbor1(previous);
        if (t != 0) {
            previous->setNeighbor2(current);
        }
        previous = current;

        if (sampleCounter < numSample - 1) {
            current = new Vertex(0,0);
        }
        t += increment;
    }

    current->setNeighbor2(0);
}

void Profile::initProfileBezier(int numSample)
{
    float p0X(0.0f);
    float p0Y(0.0f);
    float c0X(0.0f);
    float c0Y(1.0f);
    float p1X(1.0f);
    float p1Y(1.0f);
    float c1X(-1.0f);
    float c1Y(1.0f);

    float w(0.0f);
    float z(0.0f);
    float increment = 1.0f/(float)(numSample - 1);

    Vertex* current = pVertex;
    Vertex* previous = 0;
    float t(0.0f);

    for(int sampleCounter(0); sampleCounter < numSample; sampleCounter++) {
        w = pow(1.0f - t, 3) * p0X + 3.0f * pow(1.0f - t, 2) * t * c0X + 3.0f * (1.0f - t) * t * t * c1X + t * t * t * p1X;
        z = pow(1.0f - t, 3) * p0Y + 3.0f * pow(1.0f - t, 2) * t * c0Y + 3.0f * (1.0f - t) * t * t * c1Y + t * t * t * p1Y;

        current->setX(w);
        current->setY(z);

        current->setNeighbor1(previous);
        if (t != 0) {
            previous->setNeighbor2(current);
        }
        previous = current;

        if (sampleCounter < numSample - 1) {
            current = new Vertex(0,0);
        }

        t += increment;
    }

    current->setNeighbor2(0);
}

Vertex* Profile::getProfileVertex()
{
    return pVertex;
}

void Profile::addVertexEnd(Vertex * v){
    Vertex* current = pVertex;
    while (current->getNeighbor2()!= 0){
        current = current->getNeighbor2();
    }
    current->setNeighbor2(v);
    v->setNeighbor1(current);
    current->addEdge2();
    v->setEdge1(current->getEdge2());

}

void Profile::addProfileVertex(float w, float z)
{
    Vertex* newPvertex = new Vertex(w,z);

    if(pVertex!= 0){
        Vertex* current = pVertex;

        while(current->getY() < z){
            if(current->getNeighbor2() == 0){
                break;
            }
            current = current->getNeighbor2();
        }

        if(current->getNeighbor2() != 0){
            Vertex* previous =  current->getNeighbor1();

            newPvertex->setNeighbor1(previous);
            newPvertex->setNeighbor2(current);

            current->setNeighbor1(newPvertex);
            if(previous != 0){
                previous->setNeighbor2(newPvertex);
            } else {
                pVertex = newPvertex;
            }
        } else {
            newPvertex->setNeighbor1(current);
            newPvertex->setNeighbor2(current->getNeighbor2());
            current->setNeighbor2(newPvertex);
        }

    } else {
        pVertex = newPvertex;
        pVertex->setNeighbor1(0);
        pVertex->setNeighbor2(0);
    }
}
