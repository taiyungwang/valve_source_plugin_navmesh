/*
 * nav.cpp
 *
 *  Created on: Mar 25, 2017
 */

#include "nav.h"

#include <convar.h>
#include <edict.h>
#include <iplayerinfo.h>
#include <shareddefs.h>
#include <ivdebugoverlay.h>
#include <ihandleentity.h>
#include <ivmodelinfo.h>
#include <model_types.h>
#include <server_class.h>
#include <eiface.h>

extern IPlayerInfoManager *playerinfomanager;
extern IVEngineServer	*engine;

IHandleEntity* CBaseHandle::Get() const
{
	for (unsigned int i = 0; i < playerinfomanager->GetGlobalVars()->maxEntities; i++) {
		edict_t* ent = engine->PEntityOfEntIndex(i);
		if (m_Index == ent->m_NetworkSerialNumber) {
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

bool FClassnameIs(edict_t *pEntity, const char *szClassname) {
	Assert(pEntity);
	return pEntity != NULL && !strcmp(pEntity->GetClassName(), szClassname) ?
			true : false;
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

	// if we hit a clip brush, ignore it if it is not BRUSHSOLID_ALWAYS
	if (FClassnameIs(entity, "func_brush")) {
		/*
		 * TODO: imlement get propty by classname
		CFuncBrush *brush = (CFuncBrush *) entity;
		switch (brush->m_iSolidity) {
		case CFuncBrush::BRUSHSOLID_ALWAYS:
			return false;
		case CFuncBrush::BRUSHSOLID_NEVER:
			return true;
		case CFuncBrush::BRUSHSOLID_TOGGLE:
			return (flags & WALK_THRU_TOGGLE_BRUSHES) ? true : false;
		}
		*/
	}

	IPlayerInfo *info = playerinfomanager->GetPlayerInfo(entity);
	// if we hit a breakable object, assume its walkable because we will shoot it when we touch it
	if (FClassnameIs(entity, "func_breakable") && info->GetHealth()
			&& info->GetMaxHealth() > info->GetHealth())
		return (flags & WALK_THRU_BREAKABLES) ? true : false;

	if (FClassnameIs(entity, "func_breakable_surf")
			&& info->GetMaxHealth() > info->GetHealth())
		return (flags & WALK_THRU_BREAKABLES) ? true : false;

	if (FClassnameIs(entity, "func_playerinfected_clip") == true)
		return true;

	if (nav_solid_props.GetBool() && FClassnameIs(entity, "prop_*"))
		return true;

	return false;
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

bool g_PrintProps = true;

bool UTIL_FindInSendTable(SendTable *pTable,
	const char *name,
	sm_sendprop_info_t *info,
	unsigned int offset)
{
	int props = pTable->GetNumProps();
	for (int i = 0; i < props; i++)
	{
		SendProp *prop = pTable->GetProp(i);
		const char *pname = prop->GetName();
		if (g_PrintProps)
			Msg("%d : %s\n", offset + prop->GetOffset(), pname);
		if (pname && strcmp(name, pname) == 0)
		{
			info->prop = prop;
			// for some reason offset is sometimes negative when it shouldn't be
			// so take the absolute value
			info->actual_offset = offset + abs(info->prop->GetOffset());
			return true;
		}
		if (prop->GetDataTable()
				&& UTIL_FindInSendTable(prop->GetDataTable(), name, info,
						offset + prop->GetOffset())) {
			return true;
		}
	}
	return false;
}


bool UTIL_FindSendPropInfo(const ServerClass *pInfo, const char *szType, unsigned int *offset)
{
	sm_sendprop_info_t temp_info;
	if (!pInfo || !UTIL_FindInSendTable(pInfo->m_pTable, szType, &temp_info, 0))
	{
		return false;
	}
	*offset = temp_info.actual_offset;
	return true;
}

ServerClass *UTIL_FindServerClass(const char *name)
{
	extern IServerGameDLL *servergamedll;
	ServerClass *pClass = servergamedll->GetAllServerClasses();
	while (pClass)
	{
		if (strcmpi(pClass->m_pNetworkName, name) == 0)
		{
			return pClass;
		}
		pClass = pClass->m_pNext;
	}

	return NULL;
}

template<typename T>
T getClassData(edict_t* ent, const char *className, const char *type) {
	ServerClass* sc = UTIL_FindServerClass(className);
	if (sc == nullptr) {
		Msg("Could not find class %s.\n", className);
	}
	return getClassData<T>(ent, sc, type);
}


CTraceFilterSimple::CTraceFilterSimple( const IHandleEntity *passedict, int collisionGroup,
									   ShouldHitFunc_t pExtraShouldHitFunc )
{
	m_pPassEnt = passedict;
	m_collisionGroup = collisionGroup;
	m_pExtraShouldHitCheckFunction = pExtraShouldHitFunc;
}

int getTeam(edict_t* ent) {
	return getClassData<int>(ent, "CBaseEntity", "m_iTeamNum");
}

int getMoveType(edict_t *ent) {
	return getClassData<int>(ent, "CBaseEntity", "movetype") & 15;
}

bool onLadder(edict_t *ent) {
	return getMoveType(ent) == MOVETYPE_LADDER;
}

bool StandardFilterRules( IHandleEntity *pHandleEntity, int fContentsMask )
{
	edict_t *pCollide = EntityFromEntityHandle( pHandleEntity );
	// Static prop case...
	if ( !pCollide )
		return true;
	SolidType_t solid = pCollide->GetCollideable()->GetSolid();
	extern IVModelInfo *modelinfo;
	ServerClass *baseEntity = UTIL_FindServerClass("CBaseEntity");
	if ((((modelinfo->GetModelType(
			modelinfo->GetModel(
					getClassData<int>(pCollide, baseEntity, "m_nModelIndex")))
			!= mod_brush) || (solid != SOLID_BSP && solid != SOLID_VPHYSICS))
			&& (fContentsMask & CONTENTS_MONSTER) == 0)
	// This code is used to cull out tests against see-thru entities
			|| (!(fContentsMask & CONTENTS_WINDOW)
					&& getClassData<unsigned char>(pCollide, baseEntity,
							"m_nRenderMode") == kRenderNormal)
			// FIXME: this is to skip BSP models that are entities that can be
			// potentially moved/deleted, similar to a monster but doors don't seem to
			// be flagged as monsters
			// FIXME: the FL_WORLDBRUSH looked promising, but it needs to be set on
			// everything that's actually a worldbrush and it currently isn't
			|| (!(fContentsMask & CONTENTS_MOVEABLE)
					&& (getMoveType(pCollide) == MOVETYPE_PUSH)))// !(touch->flags & FL_WORLDBRUSH) )
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
	/*
	 * TODO:
	if ( !pHandleEntity )
		return false;
	if ( !pEntity->ShouldCollide( m_collisionGroup, contentsMask ) )
		return false;
	if ( pEntity && !g_pGameRules->ShouldCollide( m_collisionGroup, pEntity->GetCollisionGroup() ) )
		return false;
	if ( m_pExtraShouldHitCheckFunction &&
		(! ( m_pExtraShouldHitCheckFunction( pHandleEntity, contentsMask ) ) ) )
		return false;
	 */
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
		return engine->IndexOfEdict(pEntity) > gpGlobals->maxClients /*TODO && !player->IsNPC()*/;
	}
	return false;
}
