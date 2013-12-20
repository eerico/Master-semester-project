#ifndef EVENT_H
#define EVENT_H

class Event
{
public:
    Event(float z);
    virtual ~Event();

    virtual void handle() = 0;

    float getZ();

protected:
    float z;
};

#endif // EVENT_H
