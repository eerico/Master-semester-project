#ifndef PROFILEDESTRUCTORMANAGER_H
#define PROFILEDESTRUCTORMANAGER_H

#include <vector>
#include "Profile.h"

class ProfileDestructorManager
{
public:
    static void putProfile(Profile* p);
    static void deleteProfiles();
    static void swap();

private:
    ProfileDestructorManager();
    static std::vector<Profile*> profileManager1;
    static std::vector<Profile*> profileManager2;
    static std::vector<Profile*>* profileManager;
};

#endif // PROFILEDESTRUCTORMANAGER_H
