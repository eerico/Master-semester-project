#include "Plan.h"
#include <cmath>

Plan::Plan(Vertex *vertex1, Vertex *vertex2, Profile *profile)
    :pointX(vertex1->getX()), pointY(vertex1->getY()), pointZ(vertex1->getZ())
    , normalX(0.0f), normalY(0.0f), normalZ(1.0f)
    , pointX2(vertex2->getX()), pointY2(vertex2->getY()), pointZ2(vertex2->getZ())
    , profile(profile), vertex(vertex1)
{

}

Plan::Plan(Plan* oldPlan, Vertex* vertex1, Profile* profile)
    :normalX(0.0f), normalY(0.0f), normalZ(1.0f)
    , profile(profile), vertex(vertex1), pointX(oldPlan->pointX), pointY(oldPlan->pointY), pointZ(oldPlan->pointZ),
      pointX2(oldPlan->pointX2), pointY2(oldPlan->pointY2), pointZ2(oldPlan->pointZ2)
{

}

Plan::Plan(float vertexX, float vertexY, float vertexZ,
           float normalX, float normalY, float normalZ)
    :normalX(normalX), normalY(normalY), normalZ(normalZ)

{
    vertex = new Vertex(vertexX, vertexY, vertexZ);
}

Intersection Plan::intersect3Plans(Plan* plan2, Plan* plan3)
{
    Intersection intersection;
    intersection.eventType = General;


    float p1 = vertex->getX();
    float p2 = vertex->getY();
    float p3 = vertex->getZ();
    float n1 = normalX;
    float n2 = normalY;
    float n3 = normalZ;

    float p1_p = plan2->vertex->getX();
    float p2_p = plan2->vertex->getY();
    float p3_p = plan2->vertex->getZ();
    float n1_p = plan2->normalX;
    float n2_p = plan2->normalY;
    float n3_p = plan2->normalZ;

    float p1_pp = plan3->vertex->getX();
    float p2_pp = plan3->vertex->getY();
    float p3_pp = plan3->vertex->getZ();
    float n1_pp = plan3->normalX;
    float n2_pp = plan3->normalY;
    float n3_pp = plan3->normalZ;

    // we can write the plan equation like (xVector - pVector) * nVector = 0,
    // but we can also write it: x * nx + y * ny + z * nz = px * nx + py * ny + pz * nz = pn
    // and with 3 plan, we can write a system of 3 equations, giving us a 3x3 matrix that
    // we can solve
    // First we can find the inverse of the matrix using (1/det matrix) * adj(matrix) = inverse(matrix)

    // determinant computation:
    // | a b c |
    // | d e f | = a (e*i - h*f) - b(d*i - g*f) + c(d*h -g*e)
    // | g h i |

    float c11 = n2_p * n3_pp - n2_pp * n3_p;
    float c21 = -(n1_p * n3_pp - n1_pp * n3_p);
    float c31 = n1_p * n2_pp - n1_pp * n2_p;

    float det = n1 * c11 + n2 * c21 + n3 * c31;

    if(std::abs(det) < 0.0001f) {
        intersection.eventType = NoIntersection;
        return intersection;
    }

    // compute the classical adjoint of the matrix
    // first compute all of the co factors, giving us the cofactor matrix
    // | c11 c21 c31 |
    // | c12 c22 c32 |
    // | c13 c23 c33 |

    float c12 = -(n2 * n3_pp - n2_pp * n3);
    float c22 = n1 * n3_pp - n1_pp * n3;
    float c32 = -(n1 * n2_pp - n1_pp * n2);
    float c13 = n2 * n3_p - n2_p * n3;
    float c23 = -(n1 * n3_p - n1_p * n3);
    float c33 = n1 * n2_p - n1_p * n2;

    // the classical adjoint of the matrix is the transpose of the matrix of cofactors
    // | c11 c12 c13 |
    // | c21 c22 c23 |
    // | c31 c32 c33 |
    //
    // Now we can find x1, x2 and x3 using the invers of the matrix:
    // |x1|          | c11 c12 c13 |   |pn1|
    // |x2| = (1/det)| c21 c22 c23 | * |pn2|
    // |x3|          | c31 c32 c33 |   |pn2|

    float invDet = 1.0f / det;
    float pn1 = p1 * n1 + p2 * n2 + p3 * n3;
    float pn2 = p1_p * n1_p + p2_p * n2_p + p3_p * n3_p;
    float pn3 = p1_pp * n1_pp + p2_pp * n2_pp + p3_pp * n3_pp;


    float x1 = invDet * (c11 * pn1 + c12 * pn2 + c13 * pn3);
    float x2 = invDet * (c21 * pn1 + c22 * pn2 + c23 * pn3);
    float x3 = invDet * (c31 * pn1 + c32 * pn2 + c33 * pn3);

    intersection.x = x1;
    intersection.y = x2;
    intersection.z = x3;

    // the height must not be below the floor
    if (x3 < 0.0f) {
        intersection.eventType = NoIntersection;
    }

    return intersection;
}

void Plan::setNormal(float nx, float ny, float nz) {
    normalX = nx;
    normalY = ny;
    normalZ = nz;
}

void Plan::getNormal(float& nx, float& ny, float& nz) {
    nx = normalX;
    ny = normalY;
    nz = normalZ;
}

void Plan::computePlanNormal()
{
    float nx(0.0f);
    float ny(0.0f);
    float nz(0.0f);

    float a = pointX2 - pointX;
    float b = pointY2 - pointY;
    float c = 0.0f;

    Utils::normalize(a, b);

    Vertex* profileVertex = profile->getProfileVertexIterator();
    Vertex* nextProfileVertex = profileVertex->getNeighbor2();

    float w = nextProfileVertex->getX() - profileVertex->getX();
    float z = nextProfileVertex->getY() - profileVertex->getY();

    //est ce correct ou bien sa joue pas avec orientation des edges ?
    float wx = b;
    float wy = -a;

    float d = w * wx;
    float e = w * wy;
    float f = z;

    Utils::crossProduct(d, e, f, a, b, c, nx, ny, nz);
    Utils::normalize(nx, ny, nz);

    normalX = nx;
    normalY = ny;
    normalZ = nz;
}

void Plan::setVertex(Vertex *vertex) {
    this->vertex = vertex;
}
