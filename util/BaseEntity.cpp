/*
 * BaseClass.cpp
 *
 *  Created on: Apr 6, 2017
 */

#include "BaseEntity.h"

#include "EntityClassManager.h"
#include <edict.h>
#include <const.h>

bool BaseEntity::isDestroyedOrUsed() {
	return ent->IsFree() || (get<int>("m_fEffects") & EF_NODRAW);
}

BaseEntity::BaseEntity(const char* className, edict_t* ent): ent(ent) {
	extern EntityClassManager* classManager;
	classDef = classManager->getClass(className);
}
