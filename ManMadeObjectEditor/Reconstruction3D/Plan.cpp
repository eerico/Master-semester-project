#include "Plan.h"

Plan::Plan(const float &pointX, const float &pointY, const float &pointZ,
           const float &normalX, const float &normalY, const float &normalZ)
    :pointX(pointX), pointY(pointY), pointZ(pointZ), normalX(normalX), normalY(normalY), normalZ(normalZ)
{

}

Intersection Plan::intersect3Plans(const Plan& plan2, const Plan& plan3)
{
    Intersection intersection;
    intersection.eventType = General;

    float p1 = pointX;
    float p2 = pointY;
    float p3 = pointZ;
    float n1 = normalX;
    float n2 = normalY;
    float n3 = normalZ;

    float p1_p = plan2.pointX;
    float p2_p = plan2.pointY;
    float p3_p = plan2.pointZ;
    float n1_p = plan2.normalX;
    float n2_p = plan2.normalY;
    float n3_p = plan2.normalZ;

    float p1_pp = plan3.pointX;
    float p2_pp = plan3.pointY;
    float p3_pp = plan3.pointZ;
    float n1_pp = plan3.normalX;
    float n2_pp = plan3.normalY;
    float n3_pp = plan3.normalZ;

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

void Plan::setNormal(const float &nx, const float &ny, const float &nz) {
    normalX = nx;
    normalY = ny;
    normalZ = nz;
}
