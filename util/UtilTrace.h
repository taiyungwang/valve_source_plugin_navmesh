/*
 * UtilTraceLine.h
 *
 *  Created on: Apr 15, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_UTILTRACE_H_
#define UTILS_VALVE_NAVMESH_UTIL_UTILTRACE_H_

#include <IEngineTrace.h>

struct edict_t;

const Vector TRACE_MINS(-0.45, -0.45, 0);

const Vector TRACE_MAXS(0.45, 0.45, 55);

class CTraceFilterSimple: public CTraceFilter {
private:
	typedef bool (*ShouldHitFunc_t)(IHandleEntity *pHandleEntity,
			int contentsMask);
public:
	// It does have a base, but we'll never network anything below here..

	CTraceFilterSimple(const IHandleEntity *passentity, int collisionGroup,
			ShouldHitFunc_t pExtraShouldHitCheckFn = nullptr);
	virtual ~CTraceFilterSimple() {
	}

	virtual bool ShouldHitEntity(IHandleEntity *pHandleEntity,
			int contentsMask);
	virtual void SetPassEntity(const IHandleEntity *pPassEntity) {
		m_pPassEnt = pPassEntity;
	}
	virtual void SetCollisionGroup(int iCollisionGroup) {
		m_collisionGroup = iCollisionGroup;
	}

	const IHandleEntity *GetPassEntity(void) {
		return m_pPassEnt;
	}

private:
	const IHandleEntity *m_pPassEnt;
	int m_collisionGroup;
	ShouldHitFunc_t m_pExtraShouldHitCheckFunction;

};

//--------------------------------------------------------------------------------------------------------------

class CTraceFilterNoNPCsOrPlayer: public CTraceFilterSimple {
public:
	CTraceFilterNoNPCsOrPlayer(const IHandleEntity *passentity,
			int collisionGroup) :
			CTraceFilterSimple(passentity, collisionGroup) {
	}

	virtual bool ShouldHitEntity(IHandleEntity *pHandleEntity,
			int contentsMask);
};

//--------------------------------------------------------------------------------------------------------------
/**
 *  Trace filter that ignores players, NPCs, and objects that can be walked through
 */
class CTraceFilterWalkableEntities: public CTraceFilterNoNPCsOrPlayer {
public:
	CTraceFilterWalkableEntities(const IHandleEntity *passentity,
			int collisionGroup, unsigned int flags) :
			CTraceFilterNoNPCsOrPlayer(passentity, collisionGroup), m_flags(
					flags) {
	}

	virtual bool ShouldHitEntity(IHandleEntity *pServerEntity,
			int contentsMask);

private:
	unsigned int m_flags;
};

class FilterSelfAndTarget: public CTraceFilter {
public:
	// It does have a base, but we'll never network anything below here..

	FilterSelfAndTarget(const IHandleEntity *passentity1,
			const IHandleEntity *passentity2) :
			m_pPassEnt1(passentity1), m_pPassEnt2(passentity2) {
	}

	virtual ~FilterSelfAndTarget() {
	}

	virtual bool ShouldHitEntity(IHandleEntity *pHandleEntity,
			int contentsMask) {
		return pHandleEntity != m_pPassEnt1 && pHandleEntity != m_pPassEnt2;
	}

protected:
	const IHandleEntity *m_pPassEnt1, *m_pPassEnt2;

};

class FilterSelf: public FilterSelfAndTarget {
private:
public:
	FilterSelf(const IHandleEntity *passentity1,
			const IHandleEntity *passentity2) :
			FilterSelfAndTarget(passentity1, passentity2) {
	}

	// It does have a base, but we'll never network anything below here..
	bool ShouldHitEntity(IHandleEntity *pHandleEntity, int contentsMask) {
		if (pHandleEntity == m_pPassEnt1) {
			return false;
		}
		return pHandleEntity == m_pPassEnt2;
	}
};

void UTIL_Trace(const Ray_t& ray, unsigned int mask, const ITraceFilter& filter,
		trace_t *ptr);

void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd,
		unsigned int mask, ITraceFilter *pFilter, trace_t *ptr);

void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd,
		unsigned int mask, const IHandleEntity *ignore, int collisionGroup,
		trace_t *ptr);

void UTIL_TraceHull(const Vector &vecAbsStart, const Vector &vecAbsEnd,
		const Vector &hullMin, const Vector &hullMax, unsigned int mask,
		const IHandleEntity *ignore, int collisionGroup, trace_t *ptr);

void UTIL_TraceHull(const Vector &vecAbsStart, const Vector &vecAbsEnd,
		const Vector &hullMin, const Vector &hullMax, unsigned int mask,
		const ITraceFilter& Filter, trace_t *ptr);

Vector UTIL_FindGround(const Vector& loc);

bool UTIL_IsTargetHit(const Vector& start, const Vector& end, edict_t* self,
		edict_t* target);

#endif /* UTILS_VALVE_NAVMESH_UTIL_UTILTRACE_H_ */
