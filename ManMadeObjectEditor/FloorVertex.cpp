#include "FloorVertex.h"
/*
FloorVertex::FloorVertex(float x, float y, FloorEdge* edge, Profile* profile)
    : x(x), y(y), profile(profile)
{
    edge->splitEdgeAtVertex(this, edge1, edge2);
    delete edge;
}*/


FloorVertex::FloorVertex(float x, float y, Profile* profile, float n_x, float n_y)
    : Vertex( x,  y), profile(profile), nx(n_x), ny(n_y)
{
    normalize(nx, ny);
}

FloorVertex::~FloorVertex()
{

}

void FloorVertex::normalize(float &n_x, float &n_y) {
    float distTmp = n_x * n_x + n_y * n_y;
    if (distTmp != 0.0f) {
        distTmp = sqrt(distTmp);
        n_x = n_x / distTmp;
        n_y = n_y / distTmp;
    }
}

void FloorVertex::setNormal(float n_x, float n_y) {
    nx = n_x;
    ny = n_y;

    normalize(nx, ny);
}

void FloorVertex::getNormal(float &n_x, float &n_y)
{
    n_x = nx;
    n_y = ny;
}

Profile* FloorVertex::getProfile()
{
    return profile;
}

void FloorVertex::setProfile(Profile* p)
{
    profile = p;
}
