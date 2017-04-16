/*
 * EntityUtils.cpp
 *
 *  Created on: Apr 15, 2017
 */

#include "EntityUtils.h"

#include "BaseEntity.h"
#include "EntityClassManager.h"
#include <convar.h>
#include <edict.h>

FORCEINLINE bool NamesMatch(const char *pszQuery, string_t nameToMatch) {
	if (nameToMatch == NULL_STRING)
		return (!pszQuery || *pszQuery == 0 || *pszQuery == '*');
	const char *pszNameToMatch = STRING(nameToMatch);
	// If the pointers are identical, we're identical
	if (pszNameToMatch == pszQuery)
		return true;
	while (*pszNameToMatch && *pszQuery) {
		unsigned char cName = *pszNameToMatch;
		unsigned char cQuery = *pszQuery;
		// simple ascii case conversion
		if (cName != cQuery
				&& (cName - 'A' > (unsigned char) 'Z' - 'A'
						|| cName - 'A' + 'a' != cQuery)
				&& (cName - 'a' > (unsigned char) 'z' - 'a'
						|| cName - 'a' + 'A' != cQuery)) {
			break;
		}
		++pszNameToMatch;
		++pszQuery;
	}
	return (*pszQuery == 0 && *pszNameToMatch == 0)
	// @TODO (toml 03-18-03): Perhaps support real wildcards. Right now, only thing supported is trailing *
			|| *pszQuery == '*';
}

bool FClassnameIs(edict_t *pEntity, const char *szClassname) {
	Assert(pEntity);
	castable_string_t className(pEntity->GetClassName());
	if (IDENT_STRINGS(className, szClassname)) {
		return true;
	}
	return NamesMatch(szClassname, className);
}

/**
 * Return true if given entity can be ignored when moving
 */
bool IsEntityWalkable(edict_t *entity, unsigned int flags) {
	extern ConVar nav_solid_props;
	if (FClassnameIs(entity, "worldspawn")
			|| FClassnameIs(entity, "player"))
		return false;
	// if we hit a door, assume its walkable because it will open when we touch it
	if (FClassnameIs(entity, "func_door*")) {
#ifdef PROBLEMATIC	// cp_dustbowl doors dont open by touch - they use surrounding triggers
		if ( !entity->HasSpawnFlags( SF_DOOR_PTOUCH ) )
		{
			// this door is not opened by touching it, if it is closed, the area is blocked
			CBaseDoor *door = (CBaseDoor *)entity;
			return door->m_toggle_state == TS_AT_TOP;
		}
#endif // _DEBUG

		return (flags & WALK_THRU_FUNC_DOORS) ? true : false;
	}
	if (FClassnameIs(entity, "prop_door*")) {
		return (flags & WALK_THRU_PROP_DOORS) ? true : false;
	}
	extern EntityClassManager *classManager;
	// if we hit a clip brush, ignore it if it is not BRUSHSOLID_ALWAYS
	if (FClassnameIs(entity, "func_brush")) {
		EntityClass* brush = classManager->getClass("entity");
		switch(brush->getEntityVar("m_iSolidity").getVar<unsigned char>(entity)) {
		case BRUSHSOLID_ALWAYS:
			return false;
		case BRUSHSOLID_NEVER:
			return true;
		case BRUSHSOLID_TOGGLE:
			return (flags & WALK_THRU_TOGGLE_BRUSHES) ? true : false;
		}
	}
	BaseEntity info(*classManager, entity);
	// if we hit a breakable object, assume its walkable because we will shoot it when we touch it
	if (FClassnameIs(entity, "func_breakable") && info.getHealth() > 0
			&& info.getMaxHealth() > info.getHealth())
		return (flags & WALK_THRU_BREAKABLES) ? true : false;
	if (FClassnameIs(entity, "func_breakable_surf")
			&& info.getMaxHealth() > info.getHealth())
		return (flags & WALK_THRU_BREAKABLES) ? true : false;
	return FClassnameIs(entity, "func_playerinfected_clip")
			|| (nav_solid_props.GetBool() && FClassnameIs(entity, "prop_*"));
}


