#ifndef EVENT_H
#define EVENT_H

class Reconstruction3D;

/**
 * @brief The Event class
 * This class represent the base class for all type of event
 */
class Event
{
public:
    /**
     * @brief Event
     * @param z The height of the event
     */
    Event(float z);
    virtual ~Event();

    /**
     * @brief handle
     * Handle the event
     * @param reconstruction3D
     */
    virtual void handle(Reconstruction3D* reconstruction3D) = 0;

    /**
     * @brief isGeneralEvent
     * @return True if the event is a general event
     */
    virtual bool isGeneralEvent() = 0;

    float getZ();

protected:
    float z;
};

#endif // EVENT_H
