#ifndef PROFILE_H
#define PROFILE_H

#include <cmath>
#include "Vertex.h"

class Edge;

class Profile
{
public:
    Profile(bool empty);
    ~Profile();
    Vertex* getProfileVertex();
    void addProfileVertex(float w, float z);
    void addVertexEnd(Vertex * v);

private:
    Vertex* pVertex;
    void initProfileSkewedLine(int numSample);
    void initProfileBezier(int numSample);
};

#endif // PROFILE_H
