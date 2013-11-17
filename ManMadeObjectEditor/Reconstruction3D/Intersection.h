#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "../Geometry/Edge.h"

enum EventType {
    NoIntersection,
    General,
    EdgeDirection
};

struct Intersection {
    Edge* edge;

    //intersection point
    float x;
    float y;
    float z;

    EventType eventType;
};

#endif // INTERSECTION_H
