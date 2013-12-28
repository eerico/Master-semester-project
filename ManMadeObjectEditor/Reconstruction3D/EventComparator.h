#ifndef EVENTCOMPARATOR_H
#define EVENTCOMPARATOR_H
#include "Event.h"

/**
 * @brief The EventComparator class
 * Comparator of event. Used to sort the priority queue by the height value (ascending)
 */
class EventComparator
{
public:
    bool operator()(Event* event1, Event* event2) {
        return event1->getZ() > event2->getZ();
    }
};

#endif // EVENTCOMPARATOR_H
