#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include "Edge.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

/**
 * @brief The Vertex class
 * This class represents a vertex. A vertex can contains two neighbors and
 * edge between itself and the neighbors. The vertex class also stored information
 * used to draw this vertex
 */
class Vertex
{
    // Used to use OpenMesh data
    typedef OpenMesh::PolyMesh_ArrayKernelT<>  OMMesh;

public:
    Vertex(float x, float y, bool addFaces=false, bool valid = false);
    Vertex(float x, float y, float z);
    virtual ~Vertex();

    float getX();
    void setX(float i);

    float getY();
    void setY(float i);

    float getZ();    
    void setZ(float i);

    Vertex* getNeighbor1();
    void setNeighbor1(Vertex *vx);

    Vertex* getNeighbor2();    
    void setNeighbor2(Vertex *vx);

    /**
     * @brief replaceNeighbour
     * replace the old neighbor defined by oldVx by the new neighbor defined by
     * newVx
     * @param oldVx
     * @param newVx
     * @return the new edge between this vertex and newVx or 0 if it failed
     */
    Edge* replaceNeighbour(Vertex* oldVx, Vertex* newVx);

    QGraphicsEllipseItem* getEllipse();
    void setEllipse(QGraphicsEllipseItem* e);

    Edge* getEdge1();
    void setEdge1(Edge* e);

    Edge* getEdge2();   
    void setEdge2(Edge* e);

    std::vector<OMMesh::FaceHandle>* getFaces();

    /**
     * @brief addFace
     * Add a face associated with this vertex.
     * @param face
     */
    void addFace(OMMesh::FaceHandle face);

    /**
     * @brief isValid
     * @return If the vertex is valid
     */
    bool isValid();
    void setValid(bool valid);

    /**
     * @brief invalid
     * Set the vertex to be invalid
     */
    void invalid();

    /**
     * @brief removeVertex
     * Remove this vertex and link its two neighbors
     * @return The edge used to link its two neighbors or 0 if it doesnt
     * have two neighbors
     */
    Edge* removeVertex();

private:
    float x;
    float y;
    float z;

    // The ellipse used to draw the vertex
    QGraphicsEllipseItem* ellipse;

    // Its first neighbor
    Vertex *neighbor1;

    // The edge connecting this vertex with its first neighbor
    Edge* edge1;

    // Its second neighbor
    Vertex *neighbor2;

    // The edge connecting this vertex with its second neighbor
    Edge* edge2;

    // Used to know if they is faces information stored in this vertex
    bool bFaces;

    // Used to know if the vertex is valid
    bool valid;

    // Vector that store all faces
    std::vector<OMMesh::FaceHandle>* faces;

    // Used when we draw the vertex
    QPen vertexPen;
    QBrush vertexBrush;
};

#endif // VERTEX_H
