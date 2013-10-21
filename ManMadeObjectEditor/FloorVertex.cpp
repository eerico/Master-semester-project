#include "FloorVertex.h"

FloorVertex::FloorVertex(float x, float y, Profile* profile, float n_x, float n_y)
    : Vertex( x,  y), profile(profile), nx(n_x), ny(n_y)
{
    Utils::normalize(nx, ny);
}

FloorVertex::~FloorVertex()
{

}

void FloorVertex::setNormal(float n_x, float n_y) {
    nx = n_x;
    ny = n_y;

    Utils::normalize(nx, ny);
}

void FloorVertex::getNormal(float &n_x, float &n_y)
{
    n_x = nx;
    n_y = ny;
}

void FloorVertex::computeNormal()
{
    float x = this->getX();
    float y = this->getY();

    FloorVertex* previousVertex = (FloorVertex*)this->getNeighbor1();
    FloorVertex* nextVertex = (FloorVertex*)this->getNeighbor2();

    float vertexToPreviousVertexX = previousVertex->getX() - x;
    float vertexToPreviousVertexY = previousVertex->getY() - y;
    Utils::normalize(vertexToPreviousVertexX, vertexToPreviousVertexY);

    float vertexToNextVertexX = nextVertex->getX() - x;
    float vertexToNextVertexY = nextVertex->getY() - y;
    Utils::normalize(vertexToNextVertexX, vertexToNextVertexY);

    float newNormalX = -(vertexToPreviousVertexX + vertexToNextVertexX);
    float newNormalY = -(vertexToPreviousVertexY + vertexToNextVertexY);

    //which direction ?
    //check with next vertex
    float nextNormalX;
    float nextNormalY;
    nextVertex->getNormal(nextNormalX, nextNormalY);

    if (!(nextNormalX == 0.0f && nextNormalY == 0.0f) ) {

        float dotProduct = newNormalX * nextNormalX + newNormalY * nextNormalY;
        if (dotProduct < 0.0f) {
            setNormal(-newNormalX, -newNormalY);
        } else {
            setNormal(newNormalX, newNormalY);
        }
    } else {
        setNormal(newNormalX, newNormalY);
    }

}

Profile* FloorVertex::getProfile()
{
    return profile;
}

void FloorVertex::setProfile(Profile* p)
{
    profile = p;
}
