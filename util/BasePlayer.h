#pragma once

#include "BaseEntity.h"
#include <const.h>

class BasePlayer: public BaseEntity {
public:
	BasePlayer(edict_t *ent): BasePlayer("CBasePlayer", ent) {
	}

	virtual ~BasePlayer() {
	}

	virtual int getPlayerClass() {
		return -1;
	}

	virtual bool isOnLadder() {
		return getMoveType() == MOVETYPE_LADDER;
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
protected:
	BasePlayer(const char* className, edict_t* ent): BaseEntity(className, ent) {
	}
};
