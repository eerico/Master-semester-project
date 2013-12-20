#ifndef GENERALEVENT_H
#define GENERALEVENT_H

#include "Event.h"

class GeneralEvent: public Event
{
public:
    GeneralEvent();
    ~GeneralEvent();

    void handle();
};

#endif // GENERALEVENT_H
