#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsLineItem>
#include "../Utils.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <QPen>
#include <QBrush>

class Vertex;
class Profile;

/**
 * @brief The Edge class
 * This class represents an edge between two vertices. An edge can contain a profile
 * (in the case this edge is a floor plan edge) or not (if this edge is a profile edge).
 * The Edge class also stored information used to draw this edge.
 */
class Edge
{
    // Used to use OpenMesh data
	typedef OpenMesh::PolyMesh_ArrayKernelT<>  OMMesh;
public:
    Edge(Vertex* const vertex1, Vertex* const vertex2, Profile* const p = 0);
    ~Edge();


    Profile* getProfile();
    void setProfile(Profile* const p);

    Vertex* getVertex1();
    void setVertex1(Vertex* const vertex);

    Vertex* getVertex2();
    void setVertex2(Vertex* const vertex);

    void setLineItem(QGraphicsLineItem* const item);
    QGraphicsLineItem* getLineItem();

    /**
     * @brief computeLineItem
     * Create a new line item based on the first and second vertices
     * @return The new computed line item
     */
    QGraphicsLineItem* computeLineItem();

    /**
     * @brief isParallel
     * Test if two edges are parallel
     * @param edge
     * @return If the two edges are parallel
     */
    bool isParallel(Edge* const edge);

    /**
     * @brief distance
     * Compute the distance between two parallel edges
     * @param edge
     * @return The distance between two edges if they are parallel
     */
    float distance(Edge* const edge);

    /**
     * @brief distance
     * Compute the distance between the edge and the vertex
     * @param vertex
     * @return The distance between the edge and the vertex
     */
    float distance(Vertex* const vertex);

    void setNormal(OMMesh::Normal* constn);
    OMMesh::Normal* getNormal();

    /**
     * @brief isValid
     * @return if the edge is valid
     */
    bool isValid();
    void setValid(const bool& valid);

    /**
     * @brief invalid
     * Set the edge to be invalid
     */
    void invalid();

private:

    // The first vertex
    Vertex* vertex1;

    // The second vertex
    Vertex* vertex2;

    // The associated profile
    Profile* profile;

    // The line item used to draw the edge
    QGraphicsLineItem* lineItem;

    // The normal associated with this edge
    OMMesh::Normal *normal;

    // The pen used to draw the edge
    QPen edgePen;

    // If the edge is valid or not
    bool valid;
};

#endif // EDGE_H