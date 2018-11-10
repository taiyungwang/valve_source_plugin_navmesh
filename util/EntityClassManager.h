/*
 * EntityClassManager.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_ENTITYCLASSMANAGER_H_
#define UTILS_VALVE_NAVMESH_UTIL_ENTITYCLASSMANAGER_H_

#include <utlmap.h>

class EntityClass;
class IServerGameDLL;

/**
 * Manages and caches any entity class variable used.
 */
class EntityClassManager {
public:
	EntityClassManager(IServerGameDLL *servergamedll);

	~EntityClassManager();

	EntityClass* getClass(const char* name);

private:

	CUtlMap<const char*, EntityClass*> classes;
};



#endif /* UTILS_VALVE_NAVMESH_UTIL_ENTITYCLASSMANAGER_H_ */
