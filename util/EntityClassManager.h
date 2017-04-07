/*
 * EntityClassManager.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_ENTITYCLASSMANAGER_H_
#define UTILS_VALVE_NAVMESH_UTIL_ENTITYCLASSMANAGER_H_

#include <utlmap.h>

class ServerClass;
class IServerGameDLL;

class EntityClassManager {
public:
	EntityClassManager(IServerGameDLL *servergamedll);

	const ServerClass* getClass(const char* name) const;

private:

	CUtlMap<const char*, const ServerClass*> classes;
};



#endif /* UTILS_VALVE_NAVMESH_UTIL_ENTITYCLASSMANAGER_H_ */
