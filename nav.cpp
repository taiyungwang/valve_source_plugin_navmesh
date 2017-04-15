/*
 * nav.cpp
 *
 *  Created on: Mar 25, 2017
 */

#include "nav.h"

#include "util/EntityClassManager.h"
#include "util/EntityClass.h"
#include "util/EntityVar.h"
#include "util/BaseEntity.h"
#include "util/Exception.h"
#include <convar.h>
#include <edict.h>
#include <iplayerinfo.h>
#include <shareddefs.h>
#include <ivdebugoverlay.h>
#include <ihandleentity.h>
#include <ivmodelinfo.h>
#include <model_types.h>
#include <server_class.h>
#include <string_t.h>
#include <eiface.h>

extern IPlayerInfoManager *playerinfomanager;
extern IVEngineServer	*engine;
extern EntityClassManager *classManager;

IHandleEntity* CBaseHandle::Get() const
{
	for (unsigned int i = 0; i < playerinfomanager->GetGlobalVars()->maxEntities; i++) {
		edict_t* ent = engine->PEntityOfEntIndex(i);
		if (ent != nullptr && m_Index == ent->m_NetworkSerialNumber) {
			return ent->GetIServerEntity();
		}
	}
	return nullptr;
}


void UTIL_Trace(const Ray_t& ray, unsigned int mask,
		const ITraceFilter& filter, trace_t *ptr) {
	extern IEngineTrace *enginetrace;
	enginetrace->TraceRay( ray, mask, const_cast<ITraceFilter*>(&filter), ptr );
	extern ConVar r_visualizetraces;
	if( r_visualizetraces.GetBool() )
	{
		extern IVDebugOverlay* debugoverlay;
		debugoverlay->AddLineOverlay( ptr->startpos, ptr->endpos, 255, 0, 0, true, -1.0f );
	}
}

void Extent::Init(edict_t *entity) {

	entity->GetCollideable()->WorldSpaceSurroundingBounds(&lo, &hi);
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
	if (IDENT_STRINGS(className, szClassname)) {
		return true;
	}
	return NamesMatch(szClassname, className);
}

enum BrushSolidities_e {
	BRUSHSOLID_TOGGLE = 0,
	BRUSHSOLID_NEVER  = 1,
	BRUSHSOLID_ALWAYS = 2,
};
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
	IPlayerInfo *info = playerinfomanager->GetPlayerInfo(entity);
	// if we hit a breakable object, assume its walkable because we will shoot it when we touch it
	if (FClassnameIs(entity, "func_breakable") && info->GetHealth()
			&& info->GetMaxHealth() > info->GetHealth())
		return (flags & WALK_THRU_BREAKABLES) ? true : false;
	if (FClassnameIs(entity, "func_breakable_surf")
			&& info->GetMaxHealth() > info->GetHealth())
		return (flags & WALK_THRU_BREAKABLES) ? true : false;
	return FClassnameIs(entity, "func_playerinfected_clip")
			|| (nav_solid_props.GetBool() && FClassnameIs(entity, "prop_*"));
}

inline edict_t *EntityFromEntityHandle(IHandleEntity *pHandleEntity) {

	IServerUnknown *pUnk = (IServerUnknown*) pHandleEntity;
	return pUnk->GetNetworkable()->GetEdict();
}

bool CTraceFilterWalkableEntities::ShouldHitEntity(
		IHandleEntity *pServerEntity, int contentsMask) {
	if (CTraceFilterNoNPCsOrPlayer::ShouldHitEntity(pServerEntity,
			contentsMask)) {
		edict_t *pEntity = EntityFromEntityHandle(pServerEntity);
		return (!IsEntityWalkable(pEntity, m_flags));
	}
	return false;
}

struct sm_sendprop_info_t
{
	SendProp *prop;					/**< Property instance. */
	unsigned int actual_offset;		/**< Actual computed offset. */
};


CTraceFilterSimple::CTraceFilterSimple( const IHandleEntity *passedict, int collisionGroup,
									   ShouldHitFunc_t pExtraShouldHitFunc )
{
	m_pPassEnt = passedict;
	m_collisionGroup = collisionGroup;
	m_pExtraShouldHitCheckFunction = pExtraShouldHitFunc;
}

bool StandardFilterRules( IHandleEntity *pHandleEntity, int fContentsMask )
{
	edict_t *pCollide = EntityFromEntityHandle( pHandleEntity );
	// Static prop case...
	if ( !pCollide )
		return true;
	SolidType_t solid = pCollide->GetCollideable()->GetSolid();
	extern IVModelInfo *modelinfo;
	BaseEntity baseEntity(*classManager, pCollide);
	if ((((modelinfo->GetModelType(
			modelinfo->GetModel(baseEntity.getModelIndex())) != mod_brush)
			|| (solid != SOLID_BSP && solid != SOLID_VPHYSICS))
			&& (fContentsMask & CONTENTS_MONSTER) == 0)
	// This code is used to cull out tests against see-thru entities
			|| (!(fContentsMask & CONTENTS_WINDOW)
					&& baseEntity.getRenderMode() == kRenderNormal)
			// FIXME: this is to skip BSP models that are entities that can be
			// potentially moved/deleted, similar to a monster but doors don't seem to
			// be flagged as monsters
			// FIXME: the FL_WORLDBRUSH looked promising, but it needs to be set on
			// everything that's actually a worldbrush and it currently isn't
			|| (!(fContentsMask & CONTENTS_MOVEABLE)
					&& (baseEntity.getMoveType() == MOVETYPE_PUSH)))// !(touch->flags & FL_WORLDBRUSH) )
		return false;
	return true;
}

bool CTraceFilterSimple::ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
{
	if (!StandardFilterRules(pHandleEntity, contentsMask)
			|| (m_pPassEnt && pHandleEntity == m_pPassEnt)) {
		return false;
	}

	// Don't test if the game code tells us we should ignore this collision...
	edict_t *pEntity = EntityFromEntityHandle( pHandleEntity );
	if (!pHandleEntity || pEntity == nullptr)
		return false;
	if (BaseEntity(*classManager, pEntity).shouldCollide(m_collisionGroup,
			contentsMask)) {
		return false;
	}
	/*
	 * TODO:
	if ( pEntity && !g_pGameRules->ShouldCollide( m_collisionGroup, pEntity->GetCollisionGroup() ) )
		return false;
		*/
	if ( m_pExtraShouldHitCheckFunction &&
		(! ( m_pExtraShouldHitCheckFunction( pHandleEntity, contentsMask ) ) ) )
		return false;
	return true;
}

bool CTraceFilterNoNPCsOrPlayer::ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
{
	if ( CTraceFilterSimple::ShouldHitEntity( pHandleEntity, contentsMask ) )
	{
		edict_t *pEntity = EntityFromEntityHandle( pHandleEntity );
		if ( !pEntity )
			return NULL;
		/*
		 * TODO
#ifndef CLIENT_DLL
		if ( pEntity->Classify() == CLASS_PLAYER_ALLY )
			return false; // CS hostages are CLASS_PLAYER_ALLY but not IsNPC()
#endif
		 */
		extern CGlobalVars *gpGlobals;
		return engine->IndexOfEdict(pEntity) > gpGlobals->maxClients; //TODO && !player->IsNPC()
	}
	return false;
}
