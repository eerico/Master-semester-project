#ifndef EDGEEVENT_H
#define EDGEEVENT_H

#include "Event.h"

class EdgeEvent: public Event
{
public:
    EdgeEvent();
    ~EdgeEvent();

    void handle(Reconstruction3D* reconstruction3D);
    bool isGeneralEvent();
};

#endif // EDGEEVENT_H
