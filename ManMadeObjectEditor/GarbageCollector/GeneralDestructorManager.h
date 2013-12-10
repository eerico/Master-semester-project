#ifndef GENERALDESTRUCTORMANAGER_H
#define GENERALDESTRUCTORMANAGER_H

#include "GarbageCollectorObject.h"
#include <vector>

class GeneralDestructorManager
{
public:
    static void putObject(GarbageCollectorObject* object);
    static void deleteObject();

private:
    GeneralDestructorManager();
    static std::vector< GarbageCollectorObject* > destructorManager;
};

#endif // GENERALDESTRUCTORMANAGER_H
