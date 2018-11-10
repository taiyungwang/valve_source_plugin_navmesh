#pragma once

#include "EntityClass.h"
#include "EntityVar.h"

struct edict_t;

class EntityInstance {
public:
	virtual ~EntityInstance() {
	}

protected:

	EntityClass* classDef;

	edict_t* ent;

	EntityInstance(edict_t* ent, const char* className);

	template<typename T>
	T get(const char* name) {
		return classDef->getEntityVar(name).get<T>(ent);
	}

	template<typename T>
	T *getPtr(const char* name) {
		return classDef->getEntityVar(name).getPtr<T>(ent);
	}

};
