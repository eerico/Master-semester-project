#ifndef FLOORVERTEX_H
#define FLOORVERTEX_H

//#include "FloorEdge.h"
#include "Profile.h"
#include "Vertex.h"

//class FloorEdge; // special declaration to make the compiler understand that this class exist even if it is not yet compiled

class FloorVertex : public Vertex
{
public:
    FloorVertex(float x, float y, Profile* profile = 0, float n_x = 0.0f, float n_y = 0.0f);
    ~FloorVertex();
    Profile* getProfile();
    void setProfile(Profile* p);
    void setNormal(float n_x, float n_y);
    void getNormal(float& n_x, float& n_y);

private:
    Profile* profile;

    // normal
    float nx;
    float ny;
    void normalize(float& n_x, float& n_y);
};

#endif // FLOORVERTEX_H
