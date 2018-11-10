/*
 * EntityUtils.cpp
 *
 *  Created on: Apr 15, 2017
 */

#include "EntityUtils.h"

#include "BasePlayer.h"
#include "EntityClassManager.h"
#include <convar.h>
#include <shareddefs.h>
#include <eiface.h>
#include <iplayerinfo.h>
#include <edict.h>
#include <worldsize.h>
#include <server_class.h>

extern IVEngineServer *engine;
extern CGlobalVars *gpGlobals;

template<typename Func>
void forAllEntities(const Func& func,
		int startIndex = gpGlobals->maxClients + 1) {
	for (int i = startIndex; i < gpGlobals->maxEntities; i++) {
		edict_t *ent = engine->PEntityOfEntIndex(i);
		if (ent != nullptr && !ent->IsFree()) {
			func(ent);
		}
	}
}

edict_t * findEntWithSubStrInNetClassName(const char* name) {
	CUtlLinkedList<edict_t*> result;
	forAllEntities([name, &result](edict_t* ent) -> void {
		IServerNetworkable *network = ent->GetNetworkable();
		if (network == nullptr) {
			return;
		}
		const char* className = network->GetServerClass()->GetName();
		if (name == className || Q_stristr(className, name) != nullptr) {
			result.AddToTail(ent);
		}
	});
	return result.Count() > 0 ? result[0]: nullptr;
}

void findEntWithSubStrInName(const char* name,
		CUtlLinkedList<edict_t*>& result) {
	forAllEntities([name, &result](edict_t* ent) -> void {
		const char* className = ent->GetClassName();
		if (name == className || Q_stristr(className, name) != nullptr) {
			result.AddToTail(ent);
		}
	});
}

edict_t * findNearestEntity(const CUtlLinkedList<edict_t*>& ent,
		const Vector& pos, float maxRadius) {
	float mindist = pow(maxRadius, 2);
	edict_t* ret = nullptr;
	if (mindist == 0) {
		mindist = MAX_TRACE_LENGTH * MAX_TRACE_LENGTH;
	}
	FOR_EACH_LL(ent, i)
	{
		if (ent[i]->IsFree()) {
			continue;
		}
		float dist = pos.DistToSqr(ent[i]->GetIServerEntity()->GetCollideable()->GetCollisionOrigin());
		if (dist < mindist) {
			mindist = dist;
			ret = ent[i];
		}
	}
	return ret;
}

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
	return IDENT_STRINGS(className, szClassname)
			|| NamesMatch(szClassname, className);
}

bool isBreakable(edict_t* target) {
	BasePlayer player(target);
	const char* model = target->GetIServerEntity()->GetModelName().ToCStr();
	return !(player.getFlags() & FL_WORLDBRUSH)
			&& (model[13] != 'c' || model[17] != 'o' || model[20] != 'd'
					|| model[28] != 'e') // explosive
			&& player.getHealth() < 1000 && player.getHealth() > 0;
}

/**
 * Return true if given entity can be ignored when moving
 */
bool IsEntityWalkable(edict_t *entity, unsigned int flags) {
	extern ConVar nav_solid_props;
	if (FClassnameIs(entity, "worldspawn") || FClassnameIs(entity, "player"))
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
		return IsSolid(entity->GetCollideable()->GetSolid(),
				entity->GetCollideable()->GetSolidFlags());
		/*
		 EntityClass* brush = classManager->getClass("CFuncBrush");
		 switch(brush->getEntityVar("m_iSolidity").get<unsigned char>(entity)) {
		 case BRUSHSOLID_ALWAYS:
		 return false;
		 case BRUSHSOLID_NEVER:
		 return true;
		 case BRUSHSOLID_TOGGLE:
		 return (flags & WALK_THRU_TOGGLE_BRUSHES);
		 }
		 */
	}
	// if we hit a breakable object, assume its walkable because we will shoot it when we touch it
	return (BasePlayer(entity).getHealth()
			&& ((FClassnameIs(entity, "func_breakable")
					&& (flags & WALK_THRU_BREAKABLES))
					|| (FClassnameIs(entity, "func_breakable_surf")
							&& (flags & WALK_THRU_BREAKABLES))))
			|| FClassnameIs(entity, "func_playerinfected_clip")
			|| (nav_solid_props.GetBool() && FClassnameIs(entity, "prop_"));
}

edict_t* UTIL_GetListenServerEnt() {
	// no "local player" if this is a dedicated server or a single player game
	if (engine->IsDedicatedServer()) {
		Assert(!"UTIL_GetListenServerHost");
		Warning(
				"UTIL_GetListenServerHost() called from a dedicated server or single-player game.\n");
		return NULL;
	}
	return engine->PEntityOfEntIndex(1);
}

IPlayerInfo *UTIL_GetListenServerHost(void) {
	edict_t *ent = UTIL_GetListenServerEnt();
	extern IPlayerInfoManager* playerinfomanager;
	return ent == nullptr ? nullptr : playerinfomanager->GetPlayerInfo(ent);
}
