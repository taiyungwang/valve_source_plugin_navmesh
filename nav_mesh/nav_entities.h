//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// nav_entities.h
// Navigation entities
// Author: Michael S. Booth (mike@turtlerockstudios.com), January 2003

#ifndef NAV_ENTITIES_H
#define NAV_ENTITIES_H

#include "CountDownTimer.h"
#include "util/Handle.h"
#include <iserverentity.h>
#include <fmtstr.h>
#include <string_t.h>
#include <utlvector.h>
#include <utllinkedlist.h>
#include <shareddefs.h>

class CNavMesh;
class CNavArea;
struct edict_t;
struct inputdata_t;

//-----------------------------------------------------------------------------------------------------
/**
  * An entity that modifies pathfinding cost to all areas it overlaps, to allow map designers
  * to tell bots to avoid/prefer certain regions.
  */
class CFuncNavCost: public IServerEntity
{
public:

	CFuncNavCost(edict_t *ent): pEnt(ent) {
	}

	virtual void Spawn( void );
	virtual void UpdateOnRemove( void );

	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );

	bool IsEnabled( void ) const { return !m_isDisabled; }

	void CostThink( CNavMesh* TheNavMesh );

	bool IsApplicableTo( edict_t *who ) const;			// Return true if this cost applies to the given actor

	virtual float GetCostMultiplier( edict_t *who ) const	{ return 1.0f; }

protected:

	edict_t *pEnt;
	int m_team;
	bool m_isDisabled;
	string_t m_iszTags;

	int m_CollisionGroup;

	static CUtlVector< CHandle< CFuncNavCost > > gm_masterCostVector;
	static CountdownTimer gm_dirtyTimer;
	void UpdateAllNavCostDecoration( CNavMesh* TheNavMesh );

	void SetCollisionGroup( int collisionGroup );

	CUtlVector< CFmtStr > m_tags;
	bool HasTag( const char *groupname ) const;
};


//-----------------------------------------------------------------------------------------------------
class CFuncNavAvoid : public CFuncNavCost
{
public:

	virtual float GetCostMultiplier( edict_t *who ) const;		// return pathfind cost multiplier for the given actor
};


//-----------------------------------------------------------------------------------------------------
class CFuncNavPrefer : public CFuncNavCost
{
public:

	virtual float GetCostMultiplier( edict_t *who ) const;		// return pathfind cost multiplier for the given actor
};


//-----------------------------------------------------------------------------------------------------
/**
  * An entity that can block/unblock nav areas.  This is meant for semi-transient areas that block
  * pathfinding but can be ignored for longer-term queries like computing L4D flow distances and
  * escape routes.
  */
class CFuncNavBlocker
{
public:
	void Spawn();
	virtual void UpdateOnRemove( void );

	void InputBlockNav( inputdata_t &inputdata );
	void InputUnblockNav( inputdata_t &inputdata );

	bool IsBlockingNav( int teamNumber ) const;

	int DrawDebugTextOverlays( void );

	bool operator()( CNavArea *area );	// functor that blocks areas in our extent

	static bool CalculateBlocked( bool *pResultByTeam, const Vector &vecMins, const Vector &vecMaxs );

private:
	edict_t *pEnt;

	void UpdateBlocked();

	static CUtlLinkedList<CFuncNavBlocker *> gm_NavBlockers;

	void BlockNav( void );
	void UnblockNav( void );
	void toggleBlock(bool block);
	bool m_isBlockingNav[2];
	int m_blockedTeamNumber;
	bool m_bDisabled;
	Vector m_CachedMins, m_CachedMaxs;

};

#endif // NAV_ENTITIES_H
