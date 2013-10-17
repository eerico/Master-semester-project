#ifndef FLOORVERTEX_H
#define FLOORVERTEX_H

//#include "FloorEdge.h"
#include "Profile.h"
#include "Vertex.h"

//class FloorEdge; // special declaration to make the compiler understand that this class exist even if it is not yet compiled

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
