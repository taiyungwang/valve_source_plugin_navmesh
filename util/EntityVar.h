/*
 * EntityVar.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_ENTITYVAR_H_
#define UTILS_VALVE_NAVMESH_UTIL_ENTITYVAR_H_

struct edict_t;

class EntityVar {
public:

	void setOffset(int offset) {
		this->offset = offset;
	}

	template<typename T>
	T getVar(edict_t* ent) const {
		T out;
		getVarPtr(&out, ent);
		return out;
	}

	template<typename T>
	void getVarPtr(T* out, edict_t* ent) const;

private:

	int offset;
};

#endif /* UTILS_VALVE_NAVMESH_UTIL_ENTITYVAR_H_ */
