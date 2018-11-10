#pragma once

#include "EntityInstance.h"

class BasePlayer: public EntityInstance {
public:
	BasePlayer(edict_t *ent) :
			EntityInstance(ent, "CBasePlayer") {
	}

	int* getAmmo() {
		return getPtr<int>("m_iAmmo");
	}

	bool isUnderWater() {
		return get<int>("m_nWaterLevel") > 1;
	}

	int getHealth() {
		return get<int>("m_iHealth");
	}

	int getFlags() {
		return get<int>("m_fFlags");
	}

	edict_t* getGroundEntity() {
		return classDef->getEntityVar("m_hGroundEntity").getEntity(ent);
	}
};
