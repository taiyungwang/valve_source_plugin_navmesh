/*
 * CbaseClass.h
 *
 *  Created on: Apr 6, 2017
 */

#ifndef UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_
#define UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_

class CBaseEntity;
class SendTable;
struct edict_t;

class BaseEntity {
public:
	BaseEntity(edict_t* ent): ent(ent) {
	}

	virtual ~BaseEntity() {
	}

	int getTeam() {
		return get<int>("m_iTeamNum");
	}

	int getMoveType() {
		return get<int>("movetype") & 15;
	}

	unsigned char getRenderMode() {
		return get<unsigned char>("m_nRenderMode");
	}

	int getModelIndex() {
		return get<short>("m_nModelIndex");
	}

	int getHealth() {
		return get<int>("m_iHealth");
	}

	bool isUnderWater() {
		return get<int>("m_nWaterLevel") > 1;
	}

	edict_t* getGroundEntity() {
		return getEntity("m_hGroundEntity");
	}

	bool isDestroyedOrUsed();


	template<typename T>
	T get(const char* varName) const {
		return *getPtr<T>(varName);
	}

	template<typename T>
	T *getPtr(const char* varName) const {
		return reinterpret_cast<T*>(getPointer(varName));
	}

	int getFlags() {
		return get<int>("m_fFlags");
	}

	edict_t* getEntity(const char *varName);

protected:
	edict_t* ent = nullptr;

private:
	static int getOffset(const char* varName, SendTable* pTable, int offset);

	char *getPointer(const char* varName) const;
};

#endif /* UTILS_VALVE_NAVMESH_UTIL_BASEENTITY_H_ */
