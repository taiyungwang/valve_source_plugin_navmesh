#pragma once

#include "BaseEntity.h"
#include <vector.h>
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

	int getFlags() {
		return get<int>("m_fFlags");
	}

	Vector getVelocity() {
		return get<Vector>("m_vecVelocity[0]");
	}

protected:
	BasePlayer(const char* className, edict_t* ent): BaseEntity(className, ent) {
	}
};
