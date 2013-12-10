#ifndef PROFILEDESTRUCTORMANAGER_H
#define PROFILEDESTRUCTORMANAGER_H

#include <vector>
#include "../Geometry/Profile.h"

/**
 * @brief The ProfileDestructorManager class
 * A profiles garbage collector, that will destroy them when needed
 */
class ProfileDestructorManager
{
public:
    /**
     * @brief putProfile
     * Put the profile into the garbage colector
     * @param p The profile to put into the garbage collector
     */
    static void putProfile(Profile* p);

    /**
     * @brief deleteProfiles
     * Delete all profiles stored in the profile manager vector
     */
    static void deleteProfiles();

    /**
     * @brief swap
     * The garbate collector uses two profile manager. Only one is active at
     * a time when we add or delete. We can thus swap between the two
     * profile manager if we need to delete old profiles after having loaded
     * the new one
     */
    static void swap();

private:
    ProfileDestructorManager();

    // The first profile manager
    static std::vector< Profile* > profileManager1;

    // The second profile manager
    static std::vector< Profile* > profileManager2;

    // The current profile manager, that is a pointer to either
    // the first or the second profile manager
    static std::vector< Profile* >* profileManager;
};

#endif // PROFILEDESTRUCTORMANAGER_H
