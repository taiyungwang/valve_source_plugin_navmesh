/*
 * EntityVar.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_ENTITYVAR_H_
#define UTILS_VALVE_NAVMESH_UTIL_ENTITYVAR_H_

class CBaseEntity;
struct edict_t;

/**
 * Represents a server entity member variable.
 */
class EntityVar {
public:

	EntityVar() {
		offset = 0;
	}

	void setOffset(int offset) {
		this->offset = offset;
	}

	template<typename T>
	T* getPtr(edict_t *ent) {
		return getVarPtr<T>(getBaseEntity(ent));
	}

	template<typename T>
	T get(edict_t* ent) const {
		T *out = getVarPtr<T>(getBaseEntity(ent));
		if (out == nullptr) {
			throwException(ent);
		}
		return *out;
	}

	edict_t* getEntity(edict_t* ent) const;

private:
	static CBaseEntity* getBaseEntity(edict_t* ent);

	static void throwException(edict_t* ent);

	int offset;

	template<typename T>
	T *getVarPtr(CBaseEntity* ent) const {
		return ent == nullptr ?
				nullptr :
				reinterpret_cast<T*>(reinterpret_cast<char *>(ent) + offset);
	}


};

#endif /* UTILS_VALVE_NAVMESH_UTIL_ENTITYVAR_H_ */
