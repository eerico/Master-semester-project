#ifndef EVENTCOMPARATOR_H
#define EVENTCOMPARATOR_H
#include "Event.h"

class EventComparator
{
public:
    bool operator()(Event* event1, Event* event2) {
        return event1->getZ() > event2->getZ();
    }
};

#endif // EVENTCOMPARATOR_H
