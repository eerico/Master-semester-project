#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "../Geometry/Edge.h"
#include <vector>

enum EventType {
    NoIntersection,
    General,
    EdgeDirection
};

struct Intersection {
    std::vector<Edge*>* edgeVector;

    //intersection point
    float x;
    float y;
    float z;

    EventType eventType;
};

#endif // INTERSECTION_H
