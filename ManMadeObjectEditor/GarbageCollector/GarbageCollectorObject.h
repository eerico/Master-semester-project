#ifndef GARBAGECOLLECTOROBJECT_H
#define GARBAGECOLLECTOROBJECT_H

/**
 * @brief The GarbageCollectorObject class
 * Every object that use the GeneralDestructorManager as a garbage collector must be
 * an instance of this class.
 */
class GarbageCollectorObject
{
public:
    GarbageCollectorObject(){}
    virtual ~GarbageCollectorObject(){}
};

#endif // GARBAGECOLLECTOROBJECT_H
