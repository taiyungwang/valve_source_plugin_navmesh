/*
 * EntityUtils.h
 *
 *  Created on: Apr 15, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_ENTITYUTILS_H_
#define UTILS_VALVE_NAVMESH_UTIL_ENTITYUTILS_H_

class IHandleEntity;
struct edict_t;

enum BrushSolidities_e {
	BRUSHSOLID_TOGGLE = 0,
	BRUSHSOLID_NEVER  = 1,
	BRUSHSOLID_ALWAYS = 2,
};

#define WALK_THRU_PROP_DOORS		0x01
#define WALK_THRU_FUNC_DOORS		0x02
#define WALK_THRU_DOORS				(WALK_THRU_PROP_DOORS | WALK_THRU_FUNC_DOORS)
#define WALK_THRU_BREAKABLES		0x04
#define WALK_THRU_TOGGLE_BRUSHES	0x08
#define WALK_THRU_PROP_DOORS		0x01
#define WALK_THRU_FUNC_DOORS		0x02
#define WALK_THRU_DOORS				(WALK_THRU_PROP_DOORS | WALK_THRU_FUNC_DOORS)
#define WALK_THRU_EVERYTHING		(WALK_THRU_DOORS | WALK_THRU_BREAKABLES | WALK_THRU_TOGGLE_BRUSHES)

/**
 * Return true if given entity can be ignored when moving
 */
bool IsEntityWalkable( edict_t *entity, unsigned int flags );

bool FClassnameIs(edict_t *pEntity, const char *szClassname);


#endif /* UTILS_VALVE_NAVMESH_UTIL_ENTITYUTILS_H_ */
