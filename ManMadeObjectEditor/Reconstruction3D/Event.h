#ifndef EVENT_H
#define EVENT_H

class Reconstruction3D;

class Event
{
public:
    Event(float z);
    virtual ~Event();

    virtual void handle(Reconstruction3D* reconstruction3D) = 0;
    virtual bool isGeneralEvent() = 0;

    float getZ();

protected:
    float z;
};

#endif // EVENT_H
