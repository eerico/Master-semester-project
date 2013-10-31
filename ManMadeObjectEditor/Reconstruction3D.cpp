#include "Reconstruction3D.h"

Reconstruction3D::Reconstruction3D(Vertex* floorPlan, unsigned int floorPlanSize, std::vector<qglviewer::Vec *>* triangles)
    :floorPlan(floorPlan), floorPlanSize(floorPlanSize), triangles(triangles), currentHeight(0)
{

}

void Reconstruction3D::reconstruct()
{
    Vertex* currentVertex = floorPlan;
    for(unsigned int i(0); i < floorPlanSize ; ++i) {
        activePlan.push_back(currentVertex);
        currentVertex = floorPlan->getNeighbor2();
    }


    computeIntersection();
    while(priorityQueue.size() > 0) {
        Intersection event = priorityQueue.top();
        priorityQueue.pop();
        handleEvent(event);
    }
}

// voir fig. 8, calcul intersection pour l active plan
// et met dans la priority queue. Doit etre
// appeler si edge direction event
void Reconstruction3D::computeIntersection()
{
    foreach(Vertex* vertex, activePlan) {
        foreach(Vertex* vertex2, activePlan) {
            Edge* edge1 = vertex2->getEdge2();
            Edge* edge2 = vertex->getEdge1();
            Edge* edge3 = vertex->getEdge2();

            Intersection intersection = intersect(edge1, edge2, edge3, currentHeight);
            priorityQueue.push(intersection);
        }


        Intersection edgeDirectionEvent;
        Vertex* profileVertex = vertex->getEdge2()->getProfile()->getProfileVertex();
        if (profileVertex->isValid()) {
            edgeDirectionEvent.edge1 = vertex->getEdge2();
            edgeDirectionEvent.eventType = EdgeDirection;
            edgeDirectionEvent.y = profileVertex->getNeighbor2()->getY();
            priorityQueue.push(edgeDirectionEvent);
            profileVertex->invalid();// REMETTRE A VALID UNE FOIS, par exemple quand l event fais qu on passe au prochaine pVertex
        }
    }
}

Reconstruction3D::Intersection Reconstruction3D::intersect(Edge *edge1, Edge *edge2, Edge *edge3, float currentHeight)
{
    Vertex* vertex1 = edge1->getVertex1();
    float p1 = vertex1->getX();
    float p2 = currentHeight;
    float p3 = vertex1->getY();
    float n1(0.0f);
    float n2(0.0f);
    float n3(0.0f);
    sphericalToCartesian(vertex1, edge1->getVertex2(), n1, n2, n3);

    Vertex* vertex2 = edge2->getVertex1();
    float p1_p = vertex2->getX();
    float p2_p = currentHeight;
    float p3_p = vertex2->getY();
    float n1_p(0.0f);
    float n2_p(0.0f);
    float n3_p(0.0f);
    sphericalToCartesian(vertex1, edge1->getVertex2(), n1_p, n2_p, n3_p);

    Vertex* vertex3 = edge3->getVertex1();
    float p1_pp = vertex3->getX();
    float p2_pp = currentHeight;
    float p3_pp = vertex3->getY();
    float n1_pp(0.0f);
    float n2_pp(0.0f);
    float n3_pp(0.0f);
    sphericalToCartesian(vertex1, edge1->getVertex2(), n1_pp, n2_pp, n3_pp);

    float A = n1 - n1_p;
    float B = p1 * n1 / A;
    float C = p2 * n2 / A;
    float D = p3 * n3 / A;
    float E = p1_p * n1_p / A;
    float F = p2_p * n2_p / A;
    float G = p3_p * n3_p / A;
    float H = B + C + D - E - F - G;
    float I = (n2_p - n2) / A;
    float J = (n3_p - n3) / A;
    float K = I * n1 + n2 - I * n1_pp - n2_pp;
    float L = (-H * n1 + p1 * n1 + p2 * n2 + p3 * n3 + H * n1_pp - p1_pp * n1_pp - p2_pp * n2_pp - p3_pp * n3_pp) / K;
    float M = -J * n1 - n3 + J * n1_pp + n3_pp;
    float N = H + L * I;
    float Q = M * I + J;
    float R = N * n1_p - p1_p * n1_p + L * n2_p - p2_p * n2_p - p3_p * n3_p - N * n1_pp + p1_pp * n1_pp - L * n2_pp + p2_pp * n2_pp - p3_pp * n3_pp;
    float S = Q * n1_p + M * n2_p + n3_p - Q * n1_pp - M * n2_pp - n3_pp;

    float x3 = -R / S;
    float x2 = L + x3 * M;
    float x1 = N + x3 * Q;

    Intersection intersection;
    intersection.edge1 = edge1;
    intersection.edge2 = edge2;
    intersection.edge3 = edge3;
    intersection.eventType = General;
    intersection.x = x1;
    intersection.y = x2;
    intersection.z = x3;

    return intersection;
}

void ::Reconstruction3D::sphericalToCartesian(Vertex *vertex1, Vertex *vertex2, float &nx, float &ny, float &nz)
{
    float x1 = vertex1->getX();
    float y1 = vertex1->getY();

    float x2 = vertex2->getX();
    float y2 = vertex2->getY();


    float tangent = (y2 - y1) / (x2 - x1);
    float theta = std::atan(tangent);

    Vertex* profileVertex = vertex1->getEdge2()->getProfile()->getProfileVertex();
    Vertex* nextProfileVertex = profileVertex->getNeighbor2();

    x1 = profileVertex->getX();
    y1 = profileVertex->getY();

    x2 = nextProfileVertex->getX();
    y2 = nextProfileVertex->getY();

    tangent = (y2 - y1) / (x2 - x1);
    float phi = std::atan(tangent);

    nx = std::cos(theta) * std::sin(phi);
    nz = std::sin(theta) * std::sin(phi);
    ny = std::cos(phi);
}

// faire les truc magique. Pas oublier de update le active plan, et de set les profiles
// (pVertex) a la bonne hauteur
void Reconstruction3D::handleEvent(Intersection& intersection)
{

}
