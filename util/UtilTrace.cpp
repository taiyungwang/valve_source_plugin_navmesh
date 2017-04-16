/*
 * UtilTraceLine.cpp
 *
 *  Created on: Apr 15, 2017
 */

#include "UtilTrace.h"

#include "EntityUtils.h"
#include "EntityClass.h"
#include "BaseEntity.h"
#include "EntityClassManager.h"
#include "EntityVar.h"
#include <model_types.h>
#include <eiface.h>
#include <iplayerinfo.h>
#include <ivdebugoverlay.h>

extern EntityClassManager *classManager;
extern IVDebugOverlay* debugoverlay;
extern IEngineTrace *enginetrace;
extern ConVar r_visualizetraces;

inline edict_t *EntityFromEntityHandle(IHandleEntity *pHandleEntity) {
	return reinterpret_cast<IServerUnknown*>(pHandleEntity)->GetNetworkable()->GetEdict();
}

bool CTraceFilterWalkableEntities::ShouldHitEntity(IHandleEntity *pServerEntity,
		int contentsMask) {
	if (CTraceFilterNoNPCsOrPlayer::ShouldHitEntity(pServerEntity,
			contentsMask)) {
		return (!IsEntityWalkable(EntityFromEntityHandle(pServerEntity),
				m_flags));
	}
	return false;
}

CTraceFilterSimple::CTraceFilterSimple(const IHandleEntity *passedict,
		int collisionGroup, ShouldHitFunc_t pExtraShouldHitFunc) {
	m_pPassEnt = passedict;
	m_collisionGroup = collisionGroup;
	m_pExtraShouldHitCheckFunction = pExtraShouldHitFunc;
}

bool StandardFilterRules(IHandleEntity *pHandleEntity, int fContentsMask) {
	edict_t *pCollide = EntityFromEntityHandle(pHandleEntity);
	// Static prop case...
	if (!pCollide)
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

bool CTraceFilterSimple::ShouldHitEntity(IHandleEntity *pHandleEntity,
		int contentsMask) {
	if (!StandardFilterRules(pHandleEntity, contentsMask)
			|| (m_pPassEnt && pHandleEntity == m_pPassEnt)) {
		return false;
	}
	if (!pHandleEntity) {
		return false;
	}
	// Don't test if the game code tells us we should ignore this collision...
	edict_t *pEntity = EntityFromEntityHandle(pHandleEntity);
	if (pEntity == nullptr || BaseEntity(*classManager, pEntity).shouldCollide(m_collisionGroup,
			contentsMask)) {
		return false;
	}
	/*
	 * TODO:
	 if ( pEntity && !g_pGameRules->ShouldCollide( m_collisionGroup, pEntity->GetCollisionGroup() ) )
	 return false;
	 */
	if (m_pExtraShouldHitCheckFunction
			&& (!(m_pExtraShouldHitCheckFunction(pHandleEntity, contentsMask))))
		return false;
	return true;
}

bool CTraceFilterNoNPCsOrPlayer::ShouldHitEntity(IHandleEntity *pHandleEntity,
		int contentsMask) {
	if (CTraceFilterSimple::ShouldHitEntity(pHandleEntity, contentsMask)) {
		edict_t *pEntity = EntityFromEntityHandle(pHandleEntity);
		if (!pEntity)
			return nullptr;
		/*
		 * TODO
		 #ifndef CLIENT_DLL
		 if ( pEntity->Classify() == CLASS_PLAYER_ALLY )
		 return false; // CS hostages are CLASS_PLAYER_ALLY but not IsNPC()
		 #endif
		 */
		extern CGlobalVars *gpGlobals;
		extern IVEngineServer* engine;
		return engine->IndexOfEdict(pEntity) > gpGlobals->maxClients; //TODO && !player->IsNPC()
	}
	return false;
}

void UTIL_Trace(const Ray_t& ray, unsigned int mask, const ITraceFilter& filter,
		trace_t *ptr) {
	enginetrace->TraceRay(ray, mask, const_cast<ITraceFilter*>(&filter), ptr);
	if (r_visualizetraces.GetBool()) {
		debugoverlay->AddLineOverlay(ptr->startpos, ptr->endpos, 255, 0, 0,
				true, -1.0f);
	}
}

inline void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd,
		unsigned int mask, ITraceFilter *pFilter, trace_t *ptr) {
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	UTIL_Trace(ray, mask, *pFilter, ptr);
}

void UTIL_TraceLine( const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask,
					 const IHandleEntity *ignore, int collisionGroup, trace_t *ptr ) {
	CTraceFilterSimple filter(ignore, collisionGroup);
	UTIL_TraceLine(vecAbsStart, vecAbsEnd, mask, &filter, ptr);
}

void UTIL_TraceHull(const Vector &vecAbsStart, const Vector &vecAbsEnd,
		const Vector &hullMin, const Vector &hullMax, unsigned int mask,
		const ITraceFilter &filter, trace_t *ptr) {
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd, hullMin, hullMax);
	UTIL_Trace(ray, mask, filter, ptr);
}

void UTIL_TraceHull(const Vector &vecAbsStart, const Vector &vecAbsEnd,
		const Vector &hullMin, const Vector &hullMax, unsigned int mask,
		const IHandleEntity *ignore, int collisionGroup, trace_t *ptr) {
	UTIL_TraceHull(vecAbsStart, vecAbsEnd, hullMin, hullMax, mask,
			CTraceFilterSimple(ignore, collisionGroup), ptr);
}
