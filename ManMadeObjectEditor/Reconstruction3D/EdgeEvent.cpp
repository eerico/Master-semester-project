#include "EdgeEvent.h"
#include "Reconstruction3D.h"

EdgeEvent::EdgeEvent(float z, Edge* edge)
    :Event(z)
{
    edges.push_back(edge);
}

EdgeEvent::~EdgeEvent()
{

}

void EdgeEvent::handle(Reconstruction3D *reconstruction3D)
{
    Plan horizontalPlan(0.0f, 0.0f, z, 0.0f, 0.0f, 1.0f);

    // for each edge associated with this edge event
    foreach(Edge* edge, edges) {
        // we will compute its two vertices at the height of the edge event

        // previousEdge(plan1) - currentEdge(currentPlan) - nextEdge(plan2)
        Vertex* vertex1 = edge->getVertex1();
        Vertex* vertex2 = edge->getVertex2();

        Plan* plan1 = vertex1->getEdge1()->getDirectionPlan();
        Plan* currentPlan = edge->getDirectionPlan();
        Plan* plan2 = vertex2->getEdge2()->getDirectionPlan();

        // if the first vertex is below the edge event
        if(vertex1->getZ() < z) {
            // we compute its new position
            GeneralEvent* newIntersection1 = horizontalPlan.intersect3Plans(plan1, currentPlan);
            if(newIntersection1 == 0 ) {
                std::cerr << "Error 14" << std::endl;
                (*reconstruction3D->error) = true;
                return;
            }

            Vertex newIntersectionVertex1(newIntersection1->getX(), newIntersection1->getY(), newIntersection1->getZ());
            addNewTriangle(reconstruction3D, vertex1, vertex2, &newIntersectionVertex1);
            addNewTriangle(reconstruction3D, vertex1->getEdge1()->getVertex1(), vertex1, &newIntersectionVertex1);

            vertex1->setX(newIntersection1->getX());
            vertex1->setY(newIntersection1->getY());
            vertex1->setZ(newIntersection1->getZ());

            currentPlan->setVertex(vertex1);
        }

        // if the second vertex is below the edge event
        if(vertex2->getZ() < z){
            // we compute its new position
            GeneralEvent* newIntersection2 = horizontalPlan.intersect3Plans(currentPlan, plan2);

            if(newIntersection2 == 0 ) {
                std::cerr << "Error 15" << std::endl;
                (*reconstruction3D->error) = true;
                return;
            }
            Vertex newIntersectionVertex2(newIntersection2->getX(), newIntersection2->getY(), newIntersection2->getZ());

            addNewTriangle(reconstruction3D, vertex1, vertex2, &newIntersectionVertex2);
            addNewTriangle(reconstruction3D, vertex2, vertex2->getEdge2()->getVertex2(), &newIntersectionVertex2);


            vertex2->setX(newIntersection2->getX());
            vertex2->setY(newIntersection2->getY());
            vertex2->setZ(newIntersection2->getZ());
            plan2->setVertex(vertex2);
        }
    }

    // we update the direction plan
    Edge* firstEdge = edges[0];
    firstEdge->getProfile()->nextDirectionPlan();

    // update the normal of all direction plan
    foreach(Edge* edge, edges) {
        edge->getDirectionPlan()->computePlanNormal();
    }
}

bool EdgeEvent::isGeneralEvent() {
    return false;
}

Edge *EdgeEvent::getFirstEdge()
{
    return edges[0];
}

std::vector<Edge *> *EdgeEvent::getEdges()
{
    return &edges;
}

void EdgeEvent::addEdge(Edge *edge)
{
    edges.push_back(edge);
}

void EdgeEvent::addNewTriangle(Reconstruction3D* reconstruction3D, Vertex *vertex1, Vertex *vertex2, Vertex *vertex3) {
    qglviewer::Vec* triangleVertex1 = new qglviewer::Vec(vertex1->getX(), vertex1->getY(), vertex1->getZ());
    qglviewer::Vec* triangleVertex2 = new qglviewer::Vec(vertex2->getX(), vertex2->getY(), vertex2->getZ());
    qglviewer::Vec* triangleVertex3 = new qglviewer::Vec(vertex3->getX(), vertex3->getY(), vertex3->getZ());

    reconstruction3D->triangles->push_back(triangleVertex1);
    reconstruction3D->triangles->push_back(triangleVertex2);
    reconstruction3D->triangles->push_back(triangleVertex3);
}

