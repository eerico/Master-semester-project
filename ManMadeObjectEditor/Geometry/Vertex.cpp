#include "Vertex.h"


Vertex::Vertex(float x, float y, bool addFaces, bool valid)
    : x(x), y(y), z(0.0f), neighbor1(0), neighbor2(0), edge1(0), edge2(0), ellipse(0), valid(valid)
{
    vertexPen.setWidth(5);
    vertexPen.setColor(Qt::white);
    vertexBrush.setColor(Qt::red);
    vertexBrush.setStyle(Qt::SolidPattern);
    bFaces = addFaces;
    if(addFaces){
        faces = new std::vector<OMMesh::FaceHandle>;
    }

}

Vertex::Vertex(float x, float y, float z)
    : x(x), y(y), z(z), bFaces(false)
{

}

Vertex::~Vertex()
{
    if(bFaces){
        faces->clear();
        delete faces;
    }

}

QGraphicsEllipseItem* Vertex::getEllipse(){
    return ellipse;
}

void Vertex::setNeighbor1(Vertex* vx) {
    neighbor1 = vx;
}

void Vertex::setNeighbor2(Vertex* vx) {
    neighbor2 = vx;
}

Edge* Vertex::replaceNeighbour(Vertex* oldVx, Vertex* newVx) {
    Edge* newEdge(0);

    if(oldVx == neighbor1){
        neighbor1 = newVx;
        edge1 = new Edge(neighbor1, this);
        newEdge = edge1;
    } else if (oldVx == neighbor2){
        neighbor2 = newVx;
        edge2 = new Edge(this, neighbor2);
        newEdge = edge2;
    } else {
        std::cerr << "Something wrong! tried to add a point between 2 points that are not neighbour" << std::endl;
    }
    return newEdge;
}

Edge* Vertex::removeVertex() {
    Edge* edge = 0;

    if (neighbor1 != 0 && neighbor2 != 0) {
        // the vertex has two neighbors. Thus an edge is created between these two neighbors
        // and they are informed about their neighboring relationship
        neighbor1->setNeighbor2(neighbor2);
        neighbor2->setNeighbor1(neighbor1);

        edge = new Edge(neighbor1, neighbor2);

        neighbor1->setEdge2(edge);
        neighbor2->setEdge1(edge);
    } else if (neighbor1 != 0 && neighbor2 == 0) {
        // the vertex has only neighbor 1
        neighbor1->setNeighbor2(neighbor2);
        neighbor1->setEdge2(edge);
    } else if (neighbor2 != 0 && neighbor1 == 0){
        // the vertex has only neighbor 1
        neighbor2->setNeighbor1(neighbor1);
        neighbor2->setEdge1(edge);
    }
    return edge;
}

float Vertex::getX() {
    return x;
}

float Vertex::getY() {
    return y;
}

float Vertex::getZ() {
    return z;
}

void Vertex::setY(float i) {
    y = i;
}

void Vertex::setX(float i) {
    x = i;
}

void Vertex::setZ(float i) {
    z = i;
}

Vertex* Vertex::getNeighbor1() {
    return neighbor1;
}

Vertex* Vertex::getNeighbor2() {
    return neighbor2;
}

Edge* Vertex::getEdge1() {
    return edge1;
}

Edge* Vertex::getEdge2() {
    return edge2;
}

void Vertex::setEdge1(Edge* e) {
    edge1 = e;
}

void Vertex::setEdge2(Edge* e) {
    edge2 = e;
}

void Vertex::setEllipse(QGraphicsEllipseItem* e) {
    ellipse = e;
    ellipse->setPen(vertexPen);
    ellipse->setBrush(vertexBrush);
    ellipse->setZValue(Utils::getZValueForeground());
}

void Vertex::addFace(OMMesh::FaceHandle face) {
    faces->push_back(face);
}

std::vector<OpenMesh::PolyMesh_ArrayKernelT<>::FaceHandle >* Vertex::getFaces() {
    return faces;
}

bool Vertex::isValid() {
    return valid;
}

void Vertex::invalid() {
    valid = false;
}

void Vertex::setValid(bool valid) {
    this->valid = valid;
}

float Vertex::distance(Vertex *vertex) {
    float deltaX = x - vertex->getX();
    float deltaY = y - vertex->getY();
    float deltaZ = z - vertex->getZ();

    return std::sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
}

std::ostream& operator<<(std::ostream& out, Vertex& v) {
    out << v.getX() << "\t" << v.getY() << "\t" << v.getZ();
    return out;
}
