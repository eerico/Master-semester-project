#ifndef GENERALEVENT_H
#define GENERALEVENT_H

#include "Event.h"

class GeneralEvent: public Event
{
public:
    float getX();
    float getY();
    GeneralEvent(float x, float y, float z);
    ~GeneralEvent();

    void handle();

private:
    float x;
    float y;
};

#endif // GENERALEVENT_H
