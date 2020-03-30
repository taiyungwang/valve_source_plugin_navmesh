/*
 * CbaseClass.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_
#define UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_

#include "EntityClass.h"
#include "EntityVar.h"

struct edict_t;

class BaseEntity {
public:
	BaseEntity(edict_t* ent): BaseEntity("CBaseEntity", ent) {
	}

	virtual ~BaseEntity() {
	}

	int getTeam() {
		return get<int>("m_iTeamNum");
	}

	int getMoveType() {
		return get<int>("movetype") & 15;
	}

	unsigned char getRenderMode() {
		return get<unsigned char>("m_nRenderMode");
	}

	int getModelIndex() {
		return get<short>("m_nModelIndex");
	}

	bool isDestroyedOrUsed();

protected:
	BaseEntity(const char* className, edict_t* ent);

	EntityClass* classDef = nullptr;

	edict_t* ent = nullptr;

	template<typename T>
	T get(const char* name) {
		return classDef->getEntityVar(name).get<T>(ent);
	}

	template<typename T>
	T *getPtr(const char* name) {
		return classDef->getEntityVar(name).getPtr<T>(ent);
	}
};

#endif /* UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_ */
