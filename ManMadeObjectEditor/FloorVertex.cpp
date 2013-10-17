#include "FloorVertex.h"
/*
FloorVertex::FloorVertex(float x, float y, FloorEdge* edge, Profile* profile)
    : x(x), y(y), profile(profile)
{
    edge->splitEdgeAtVertex(this, edge1, edge2);
    delete edge;
}*/


FloorVertex::FloorVertex(float x, float y, Profile* profile)
    : Vertex( x,  y), profile(profile)
{

}

FloorVertex::~FloorVertex()
{
 //delete profile
}
Profile* FloorVertex::getProfile()
{
    return profile;
}

void FloorVertex::setProfile(Profile* p)
{
    profile = p;
}
