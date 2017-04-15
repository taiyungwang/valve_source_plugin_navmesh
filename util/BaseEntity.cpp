/*
 * BaseClass.cpp
 *
 *  Created on: Apr 6, 2017
 */

#include "BaseEntity.h"

#include "EntityClassManager.h"
#include <edict.h>
#include <const.h>

BaseEntity::BaseEntity(EntityClassManager& classManager, edict_t* ent) :
		ent(ent) {
	classDef = classManager.getClass("CBaseEntity");
}

bool BaseEntity::isOnLadder() {
	return getMoveType() == MOVETYPE_LADDER;
}

bool BaseEntity::shouldCollide(int collisionGroup, int contentsMask) {
	return get<int>("m_Collision") != COLLISION_GROUP_DEBRIS
			|| (contentsMask & CONTENTS_DEBRIS);
}
