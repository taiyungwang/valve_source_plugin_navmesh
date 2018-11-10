/*
 * BaseClass.cpp
 *
 *  Created on: Apr 6, 2017
 */

#include "BaseEntity.h"

#include <edict.h>
#include <const.h>

bool BaseEntity::isOnLadder() {
	return getMoveType() == MOVETYPE_LADDER;
}

bool BaseEntity::isDestroyedOrUsed() {
	return ent->IsFree() || (get<int>("m_fEffects") & EF_NODRAW);
}
