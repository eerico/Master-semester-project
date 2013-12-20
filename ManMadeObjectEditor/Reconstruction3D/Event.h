#ifndef EVENT_H
#define EVENT_H

class Event
{
public:
    Event();
    virtual ~Event();

    virtual void handle() = 0;
};

#endif // EVENT_H
