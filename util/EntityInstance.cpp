#include "EntityInstance.h"

#include "EntityClassManager.h"

EntityInstance::EntityInstance(edict_t* ent, const char* className) :
		ent(ent) {
	extern EntityClassManager* classManager;
	classDef = classManager->getClass(className);
}
