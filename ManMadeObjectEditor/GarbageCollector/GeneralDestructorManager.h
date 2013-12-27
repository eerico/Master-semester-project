#ifndef GENERALDESTRUCTORMANAGER_H
#define GENERALDESTRUCTORMANAGER_H

#include "GarbageCollectorObject.h"
#include <vector>

/**
 * @brief The GeneralDestructorManager class
 * This class is used as a garbage collector during the 3D reconstruction.
 * All object used with this garbage collector must be a GarbageCollectorObject.
 *
 */
class GeneralDestructorManager
{
public:
    /**
     * @brief putObject
     * Put the object in the garbage collector
     * @param object The object to put into the garbage collector
     */
    static void putObject(GarbageCollectorObject* object);

    /**
     * @brief deleteObject
     * Delete all objects stored in the garbage collector
     */
    static void deleteObject();

private:
    GeneralDestructorManager();

    // the garbage collector
    static std::vector< GarbageCollectorObject* > destructorManager;
};

#endif // GENERALDESTRUCTORMANAGER_H
