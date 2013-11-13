#ifndef PROFILE_H
#define PROFILE_H

#include <cmath>
#include <QColor>
#include "Vertex.h"
#include "Utils.h"

class Edge;

/**
 * @brief The Profile class
 * This class represent a profile associated with an edge
 */
class Profile
{
public:
    Profile(bool empty);
    ~Profile();

    Vertex* getProfileVertex();

    /**
     * @brief addProfileVertex
     * Add a vertex in the profile
     * @param w The inclination
     * @param z The height
     */
    void addProfileVertex(float w, float z);

    /**
     * @brief addVertexEnd
     * Add a vertex at the end of this profile
     * @param v
     * @return true if the method succeeded (do not break the monotonicity)
     */
    bool addVertexEnd(Vertex * v);

    /**
     * @brief vertexDecimation
     * Reduce the number of vertices defining this profile without loosing
     * any details
     */
    void vertexDecimation();

    /**
     * @brief isEqual
     * @param compareProfile
     * @return true if the two profiles are the same
     */
    bool isEqual(Profile* compareProfile);

    /**
     * @brief nextDirectionPlan
     * Set the current direction plan to be the next one
     */
    void nextDirectionPlan();

    /**
     * @brief resetDirectionPlan
     * reset the currrent direction plan to be the first one
     */
    void resetDirectionPlan(); 

    QColor* getProfileColorIdentification();
    
private:

    // the vertex at the current direction plan
    Vertex* pVertex;

    // the color identification of this profile
    QColor* profileColorIdentification;

    /**
     * @brief initProfileSkewedLine
     * Create a profile using the skewed line
     * @param numSample The number of sample to take
     */
    void initProfileSkewedLine(int numSample);

    /**
     * @brief initProfileBezier
     * Create a profile using a bezier curve
     * @param numSample The number of sample to take
     */
    void initProfileBezier(int numSample);

};

#endif // PROFILE_H
