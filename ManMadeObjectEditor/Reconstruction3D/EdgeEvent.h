#ifndef EDGEEVENT_H
#define EDGEEVENT_H

#include "Event.h"

class EdgeEvent: public Event
{
public:
    EdgeEvent();
    ~EdgeEvent();

    void handle();
};

#endif // EDGEEVENT_H
