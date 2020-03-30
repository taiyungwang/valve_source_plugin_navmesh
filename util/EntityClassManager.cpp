/*
 * EntityClassManager.cpp
 *
 *  Created on: Apr 6, 2017
 */

#include "EntityClassManager.h"

#include "EntityClass.h"
#include "EntityVar.h"
#include "SimpleException.h"
#include <edict.h>
#include <server_class.h>
#include <eiface.h>

EntityClassManager::EntityClassManager(IServerGameDLL *servergamedll) {
	SetDefLessFunc(classes);
	for (const ServerClass *pClass = servergamedll->GetAllServerClasses();
			pClass != nullptr; pClass = pClass->m_pNext)
	{
		classes.Insert(pClass->m_pNetworkName, new EntityClass(pClass));
	}
}

EntityClassManager::~EntityClassManager() {
	classes.Purge();
}

EntityClass* EntityClassManager::getClass(const char* name) {
	int i = classes.Find(name);
	if (classes.IsValidIndex(i)) {
		return classes.Element(i);
	}
	throw SimpleException(CUtlString("Class not found: ") + name);
}
