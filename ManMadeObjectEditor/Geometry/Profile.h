#ifndef PROFILE_H
#define PROFILE_H

#include <cmath>
#include <QColor>
#include "../Geometry/Vertex.h"
#include "../Utils.h"
#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>


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
    bool isEqual(Profile * compareProfile);

    /**
     * @brief isEquivalent
     * @param compareProfile
     * @return return the "most complete profile" if the two profile are considered equivalent 0 otherwise.
     *most complete profile and equivalent means that one profile is contained in the other and we return the biggest one.
     */
    Profile* isEquivalent(Profile* compareProfile);

    /**
     * @brief nextDirectionPlan
     * Set the current direction plan to be the next one
     */
    void nextDirectionPlan();

    /**
     * @brief resetDirectionPlan
     * reset the current direction plan to be the first one
     */
    void resetDirectionPlan(); 

    QColor* getProfileColorIdentification();

    /**
     * @brief getProfileVertexIterator
     * @return The iteration used to know the current direction plan
     * of this profile
     */
    Vertex* getProfileVertexIterator();

    /**
     * @brief copy
     * @return a copy of this profile
     */
    Profile* copy();

    void printProfile();

    void writeXML(QXmlStreamWriter* xmlWriter);
    static std::pair<QString, Profile*> readXML(QXmlStreamReader &xml);


private:

    // the vertex at the current direction plan
    Vertex* pVertex;

    // an iterator used to know the current direction plan when we do the
    // 3D reconstruction
    Vertex* profileVertexIterator;

    // the color identification of this profile
    QColor* profileColorIdentification;

    /**
     * @brief initProfileSkewedLine
     * Create a profile using the skewed line
     * @param numSample The number of sample to take
     */
    void initProfileSkewedLine(int numSample);

    /** DEPRECATED
     * @brief initProfileBezier
     * Create a profile using a bezier curve
     * @param numSample The number of sample to take
     */
    void initProfileBezier(int numSample);

};

#endif // PROFILE_H
