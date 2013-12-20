#ifndef INTERSECTIONCOMPARATOR_H
#define INTERSECTIONCOMPARATOR_H
#include "Intersection.h"

class IntersectionComparator
{
public:
    bool operator()(Intersection& intersection1, Intersection& intersection2){
        return intersection1.z > intersection2.z;
    }
};

#endif // INTERSECTIONCOMPARATOR_H
