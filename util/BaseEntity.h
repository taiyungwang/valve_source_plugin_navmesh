/*
 * CbaseClass.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_
#define UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_

class EntityClassManager;
class EntityClass;
struct edict_t;

class BaseEntity {
public:
	BaseEntity(const EntityClassManager& classManager, edict_t *ent);

	~BaseEntity();

	int getITeam();

	unsigned char getMoveType();

	unsigned char getRenderNormal();

	unsigned char getRenderMode();
private:

	template<typename T>
	T get(const char* name);

	EntityClass* classDef;

	edict_t* ent;
};

#endif /* UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_ */
