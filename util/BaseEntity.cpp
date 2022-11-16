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
	int offset = getOffset(varName, ent->m_pNetworkable->GetServerClass()->m_pTable, 0);
	if (offset < 0) {
		throw SimpleException(CUtlString("Unable find offset for variable, ")
					+ varName + ", for class, "
					+ ent->GetClassName() + ".\n");
	}
	return ent->GetUnknown() == nullptr
			? nullptr : (reinterpret_cast<char *>(ent->GetUnknown()->GetBaseEntity()) + offset);
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

void BaseEntity::throwException(const char *varName) const {
	throw SimpleException(CUtlString("Unable find entity variable, ")
			+ varName + ", for class, "
			+ ent->GetClassName() + ".\n");
}

