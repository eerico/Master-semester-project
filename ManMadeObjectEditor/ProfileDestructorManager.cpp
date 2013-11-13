#include "ProfileDestructorManager.h"

std::vector<const Profile* const> ProfileDestructorManager::profileManager1;
std::vector<const Profile* const> ProfileDestructorManager::profileManager2;
std::vector<const Profile* const>* ProfileDestructorManager::profileManager(&ProfileDestructorManager::profileManager1);

void ProfileDestructorManager::putProfile(const Profile * const p) {
    profileManager->push_back(p);
}

void ProfileDestructorManager::deleteProfiles() {
    unsigned int size = profileManager->size();
    for (unsigned int i(0); i < size; ++i) {
        delete (*profileManager)[i];
    }
    profileManager->clear();
}

void ProfileDestructorManager::swap() {
    if (profileManager == &profileManager1) {
        profileManager = &profileManager2;
    } else {
        profileManager = &profileManager1;
    }
}
