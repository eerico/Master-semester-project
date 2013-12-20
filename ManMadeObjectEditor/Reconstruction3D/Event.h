#ifndef EVENT_H
#define EVENT_H

class Event
{
public:
    Event(float y);
    virtual ~Event();

    virtual void handle() = 0;

protected:
    float y;
};

#endif // EVENT_H
