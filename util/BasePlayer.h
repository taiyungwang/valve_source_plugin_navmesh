#pragma once

#include "BaseEntity.h"
#include <vector.h>
#include <const.h>

class BasePlayer: public BaseEntity {
public:
	BasePlayer(edict_t *ent): BaseEntity(ent) {
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

	Vector getVelocity() {
		return get<Vector>("m_vecVelocity[0]");
	}
};
