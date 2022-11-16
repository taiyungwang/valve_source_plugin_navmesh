/*
 * BaseClass.cpp
 *
 *  Created on: Apr 6, 2017
 */

#include "BaseEntity.h"

#include "SimpleException.h"
#include <eiface.h>
#include <server_class.h>
#include <const.h>

bool BaseEntity::isDestroyedOrUsed() {
	return ent->IsFree() || (get<int>("m_fEffects") & EF_NODRAW);
}

edict_t* BaseEntity::getEntity(const char *varName) {
	CBaseHandle* out = getPtr<CBaseHandle>(varName);
	extern IVEngineServer* engine;
	return out == nullptr ?
			nullptr : engine->PEntityOfEntIndex(out->GetEntryIndex());
}

char *BaseEntity::getPointer(const char* varName) const {
	IServerUnknown *unk = ent->GetUnknown();
	ServerClass *serverClass = ent->m_pNetworkable->GetServerClass();
	if (ent->IsFree() || !unk) {
		throw SimpleException(CUtlString("Unable to get, ") + serverClass->m_pNetworkName
				+ "." + varName + ", because entity, " + ent->GetClassName() + "is unvailable.\n");
	}
	int offset = getOffset(varName, serverClass->m_pTable, 0);
	if (offset < 0) {
		throw SimpleException(CUtlString("Unable find offset for ")
				+ serverClass->m_pNetworkName
				+ "." + varName + " for entity, " + ent->GetClassName() + ".\n");
	}
	return reinterpret_cast<char *>(unk->GetBaseEntity()) + offset;
}

int BaseEntity::getOffset(const char* varName, SendTable* pTable, int offset) {
	int props = pTable->GetNumProps();
	for (int i = 0; i < props; i++) {
		SendProp *prop = pTable->GetProp(i);
		const char *pname = prop->GetName();
		if (pname != nullptr && strcmp(varName, pname) == 0) {
			// for some reason offset is sometimes negative when it shouldn't be
			// so take the absolute value
			return offset + abs(prop->GetOffset());
		}
		if (prop->GetDataTable()) {
			int newOffset = getOffset(varName, prop->GetDataTable(),
						offset + abs(prop->GetOffset()));
			if (newOffset >= 0) {
				return newOffset;
			}
		}
	}
	return -1;
}

