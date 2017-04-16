/*
 * CbaseClass.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_
#define UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_

#include "EntityClass.h"
#include "EntityVar.h"

class EntityClassManager;
struct edict_t;

class BaseEntity {
public:
	BaseEntity(EntityClassManager& classManager, edict_t *ent);

	int getTeam() {
		return get<int>("m_iTeam");
	}

	int getMoveType() {
		return get<int>("movetype");
	}

	int getHealth() {
		return get<int>("m_iHealth");
	}

	int getMaxHealth() {
		return get<int>("m_iMaxHealth");
	}

	bool isOnLadder();

	unsigned char getRenderMode() {
		return get<unsigned char>("m_nRenderMode");
	}

	int getModelIndex() {
		return get<short>("m_nModelIndex");
	}

	bool shouldCollide(int collisionGroup, int contentsMask);

private:

	template<typename T>
	T get(const char* name) {
		return classDef->getEntityVar(name).getVar<T>(ent);
	}

	EntityClass* classDef;

	edict_t* ent;
};

#endif /* UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_ */
