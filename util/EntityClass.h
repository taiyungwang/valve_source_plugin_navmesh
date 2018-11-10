/*
 * EntityClass.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_ENTITYCLASS_H_
#define UTILS_VALVE_NAVMESH_UTIL_ENTITYCLASS_H_

#include <utlmap.h>

class EntityVar;
class SendTable;
class ServerClass;

/**
 * Defines an Entity Class. It caches any previously added class
 * variable to an internal map.
 */
class EntityClass {
public:
	EntityClass(const ServerClass* pClass) :
			pClass(pClass) {
		SetDefLessFunc(vars);

	}

	EntityVar& getEntityVar(const char* varName);

private:

	int addVar(const char* varName, SendTable* pTable, int offset);

	const ServerClass* pClass;

	CUtlMap<const char*, EntityVar> vars;
};

#endif /* UTILS_VALVE_NAVMESH_UTIL_ENTITYCLASS_H_ */
