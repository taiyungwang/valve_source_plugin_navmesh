/*
 * EntityClass.cpp
 *
 *  Created on: Apr 6, 2017
 */

#include "EntityClass.h"

#include "EntityVar.h"
#include "SimpleException.h"
#include <server_class.h>
#include <dt_send.h>
#include <utlstring.h>
#include <edict.h>

const EntityVar& EntityClass::getEntityVar(const char* varName) {
	int i = vars.Find(varName);
	if (!vars.IsValidIndex(i)) {
		i = addVar(varName, pClass->m_pTable,  0);
	}
	return vars.Element(i);

}

int EntityClass::addVar(const char* varName, SendTable* pTable,
		int offset) {
	int props = pTable->GetNumProps();
	for (int i = 0; i < props; i++) {
		SendProp *prop = pTable->GetProp(i);
		const char *pname = prop->GetName();
		if (pname != nullptr && strcmp(varName, pname) == 0) {
			// for some reason offset is sometimes negative when it shouldn't be
			// so take the absolute value
			int idx = vars.Insert(varName,
					EntityVar());
			vars.Element(idx).setOffset(offset + abs(prop->GetOffset()));
			return idx;
		}
		if (prop->GetDataTable()) {
				return addVar(varName, prop->GetDataTable(),
						offset + prop->GetOffset());
		}
	}
	throw SimpleException(CUtlString("Variable could not be found: ") + varName);
}
