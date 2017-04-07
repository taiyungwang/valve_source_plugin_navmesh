/*
 * BaseClass.cpp
 *
 *  Created on: Apr 6, 2017
 */

#include "BaseEntity.h"

#include "EntityClass.h"
#include "EntityClassManager.h"
#include "EntityVar.h"
#include <edict.h>

BaseEntity::BaseEntity(const EntityClassManager& classManager, edict_t* ent) :
		ent(ent) {
	classDef = new EntityClass(classManager.getClass("CBaseEntity"));
}

BaseEntity::~BaseEntity() {
	delete classDef;
}

int BaseEntity::getITeam() {
	return get<int>("m_iTeam");
}

unsigned char BaseEntity::getMoveType() {
	return get<unsigned char>("m_MoveType");
}

unsigned char BaseEntity::getRenderMode() {
	return get<unsigned char>("m_nRenderMode");
}


template<typename T>
T BaseEntity::get(const char* name) {
	return classDef->getEntityVar(name).getVar<T>(ent);
}
