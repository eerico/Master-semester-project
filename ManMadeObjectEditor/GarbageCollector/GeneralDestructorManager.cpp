#include "GeneralDestructorManager.h"

std::vector< GarbageCollectorObject* > GeneralDestructorManager::destructorManager;

void GeneralDestructorManager::putObject(GarbageCollectorObject *object) {
    destructorManager.push_back(object);
}

void GeneralDestructorManager::deleteObject() {
    unsigned int size = destructorManager.size();
    for (unsigned int i(0); i < size; ++i) {
        delete destructorManager[i];
    }
    destructorManager.clear();
}
