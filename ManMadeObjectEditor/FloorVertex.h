#ifndef FLOORVERTEX_H
#define FLOORVERTEX_H

//#include "FloorEdge.h"
#include "Profile.h"
#include "Vertex.h"

class FloorVertex : public Vertex
{
public:
    FloorVertex(float x, float y, Profile* profile = 0);
    ~FloorVertex();
    Profile* getProfile();
    void setProfile(Profile* p);

private:
    Profile* profile;
};

#endif // FLOORVERTEX_H
