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

    std::vector<Edge*>* edgeVector;

    //intersection point
    float x;
    float y;
    float z;

    EventType eventType;
};

#endif // INTERSECTION_H
