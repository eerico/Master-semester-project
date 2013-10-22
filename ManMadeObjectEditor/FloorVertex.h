#ifndef FLOORVERTEX_H
#define FLOORVERTEX_H

#include "Profile.h"
#include "Vertex.h"
#include "Utils.h"

class FloorVertex : public Vertex
{
public:
    FloorVertex(float x, float y, Profile* profile = 0, float n_x = 0.0f, float n_y = 0.0f);
    ~FloorVertex();
    Profile* getProfile();
    void setProfile(Profile* p);
    void setNormal(float n_x, float n_y);
    void getNormal(float& n_x, float& n_y);
    void computeNormal();

private:
    Profile* profile;

    // normal
    float nx;
    float ny;
};

#endif // FLOORVERTEX_H
