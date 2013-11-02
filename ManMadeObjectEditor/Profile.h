#ifndef PROFILE_H
#define PROFILE_H

#include <cmath>
#include <QColor>
#include "Vertex.h"
#include "Utils.h"

class Edge;

class Profile
{
public:
    Profile(bool empty);
    ~Profile();
    Vertex* getProfileVertex();
    void addProfileVertex(float w, float z);
    void addVertexEnd(Vertex * v);
    void vertexDecimation();
    bool isEqual(Profile* compareProfile);
    void nextDirectionPlan();
    void resetDirectionPlan();
    
    QColor* getProfileColorIdentification();

    //the orientation angle (spherical coordinate) theta
    void setSphericalCoordinatesTheta(float newTheta);
    float getSphericalCoordinatesTheta();
    
private:
    Vertex* pVertex;
    void initProfileSkewedLine(int numSample);
    void initProfileBezier(int numSample);
    QColor* profileColorIdentification;
};

#endif // PROFILE_H
