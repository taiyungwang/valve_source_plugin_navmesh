/*
 * EntityClassManager.cpp
 *
 *  Created on: Apr 6, 2017
 */

#include "EntityClassManager.h"

#include <edict.h>
#include <server_class.h>
#include <eiface.h>

EntityClassManager::EntityClassManager(IServerGameDLL *servergamedll) {
	SetDefLessFunc(classes);
	for (const ServerClass *pClass = servergamedll->GetAllServerClasses();
			pClass != nullptr; pClass = pClass->m_pNext)
	{
		classes.Insert(pClass->m_pNetworkName, pClass);
	}
}

const ServerClass* EntityClassManager::getClass(const char* name) const {
	int i = classes.Find(name);
	if (classes.IsValidIndex(i)) {
		return classes.Element(i);
	}
	return nullptr;
}
