/*
 * CbaseClass.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_
#define UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_

#include "EntityInstance.h"


class BaseEntity: public EntityInstance {
public:
	BaseEntity(edict_t *ent): EntityInstance(ent, "CBaseEntity") {
	}

	int getTeam() {
		return get<int>("m_iTeamNum");
	}

	int getMoveType() {
		return get<int>("movetype") & 15;
	}

	bool isOnLadder();

	unsigned char getRenderMode() {
		return get<unsigned char>("m_nRenderMode");
	}

	int getModelIndex() {
		return get<short>("m_nModelIndex");
	}

	bool isDestroyedOrUsed();
};

#endif /* UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_ */
