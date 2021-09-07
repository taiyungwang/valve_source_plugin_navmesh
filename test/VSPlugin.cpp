#include "VSPlugin.h"

#include <util/EntityClassManager.h>
#include <IEngineTrace.h>
#include <eiface.h>
#include <ivdebugoverlay.h>
#include <filesystem.h>
#include <tier1.h>
#include <tier2/tier2.h>
#include <iplayerinfo.h>
#include <interface.h>
#include <vphysics_interface.h>
#include <datacache/imdlcache.h>
#include <nav_mesh/nav_mesh.h>

IBotManager *botmanager = nullptr;
IVDebugOverlay *debugoverlay = nullptr;
IVEngineServer* engine = nullptr;
IServerGameDLL *servergamedll = nullptr;
IEngineTrace *enginetrace = nullptr;
IServerPluginHelpers *helpers = nullptr;
IPlayerInfoManager *playerinfomanager = nullptr;
IFileSystem *filesystem;
IGameEventManager2 *gameeventmanager = nullptr;
IGameEventManager *gameeventmanager1 = nullptr;
IMDLCache *mdlcache = nullptr;
IServerGameClients* gameclients = nullptr;
IPhysicsSurfaceProps *physprops = nullptr;
IVModelInfo *modelinfo = nullptr;
IPhysicsSurfaceProps *physprop = nullptr;
IServerGameEnts *servergameents = nullptr;
CGlobalVars *gpGlobals = nullptr;
EntityClassManager *classManager = nullptr;
CNavMesh* TheNavMesh = nullptr;
bool navMeshLoadAttempted;

ConVar r_visualizetraces("r_visualizetraces", "0", FCVAR_CHEAT);

//
// The plugin is a static singleton that is exported as an interface
//
VSPlugin plugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(VSPlugin, IServerPluginCallbacks,
		INTERFACEVERSION_ISERVERPLUGINCALLBACKS, plugin);

template<typename type>
bool loadUndefined(type*& var, CreateInterfaceFn gameServerFactory,
		const char* vername, unsigned int maxver, unsigned int minver) {
	char str[256];
	for (unsigned int ver = maxver; ver >= minver && var == nullptr; ver--) {
		memset(str, 0, 256);
		sprintf(str, "%s%03d", vername, ver);
		var = reinterpret_cast<type*>(gameServerFactory(str, nullptr));
		if (var != nullptr) {
			Msg("Found interface %s\n", str);
			return true;
		}
	}
	Warning("Cannot open interface %s (Max ver: %d) (Min ver: %d) \n", vername,
			maxver, minver);
	return false;
}

template<typename T>
bool load(T*& ptr, CreateInterfaceFn interfaceFactory, const char *ver) {
	ptr = reinterpret_cast<T*>(interfaceFactory(ver, nullptr));
	if (ptr == nullptr) {
		Warning("Unable to load %s.\n", ver);
		return false;
	}
	return true;
}

VSPlugin::VSPlugin() {
	m_iClientCommandIndex = 0;
}

bool VSPlugin::Load(CreateInterfaceFn interfaceFactory,
		CreateInterfaceFn gameServerFactory) {
	ConnectTier1Libraries(&interfaceFactory, 1);
	ConnectTier2Libraries(&interfaceFactory, 1);
	if (!load(mdlcache, interfaceFactory, MDLCACHE_INTERFACE_VERSION)
			|| !load(physprops, interfaceFactory, VPHYSICS_SURFACEPROPS_INTERFACE_VERSION)
			|| !load(filesystem, interfaceFactory, FILESYSTEM_INTERFACE_VERSION)
			|| !load(gameeventmanager1, interfaceFactory, INTERFACEVERSION_GAMEEVENTSMANAGER)
			|| !load(gameeventmanager, interfaceFactory, INTERFACEVERSION_GAMEEVENTSMANAGER2)
			|| !load(engine, interfaceFactory, INTERFACEVERSION_VENGINESERVER)
			|| !load(helpers, interfaceFactory, INTERFACEVERSION_ISERVERPLUGINHELPERS)
			|| !load(enginetrace, interfaceFactory, INTERFACEVERSION_ENGINETRACE_SERVER)
			|| !load(modelinfo, interfaceFactory, VMODELINFO_SERVER_INTERFACE_VERSION)
			|| (!engine->IsDedicatedServer() && !load(debugoverlay, interfaceFactory,
					VDEBUG_OVERLAY_INTERFACE_VERSION))
			|| !loadUndefined(playerinfomanager, gameServerFactory, "PlayerInfoManager", 3, 1)
			|| !loadUndefined(gameclients, gameServerFactory, "ServerGameClients", 5, 1)
			|| !loadUndefined(servergamedll, gameServerFactory, "ServerGameDLL", 12, 0)
			|| !loadUndefined(botmanager, gameServerFactory, "BotManager", 3, 1)
			|| !loadUndefined(servergameents, gameServerFactory,
					"ServerGameEnts", 10, 1)) {
		return false;
	}
	ConVar_Register(0);
	MathLib_Init();
	classManager = new EntityClassManager(servergamedll);
	gpGlobals = playerinfomanager->GetGlobalVars();
	TheNavMesh = new CNavMesh;
	char modPath[256];
	engine->GetGameDir(modPath, 256);
	if (Q_stristr(modPath, "dod")) {
		TheNavMesh->addPlayerSpawnName("info_player_axis");
		TheNavMesh->addPlayerSpawnName("info_player_allies");
	} else {
		TheNavMesh->addPlayerSpawnName("info_player_start");
	}
	return true;
}

const char *VSPlugin::GetPluginDescription(void) {
	return "Sample navmesh plugin";
}

void VSPlugin::LevelInit(char const *pMapName) {
	navMeshLoadAttempted = false;
	gameeventmanager->AddListener(this, "MyBot", true);
	gameeventmanager1->AddListener(this, true);
}

void VSPlugin::GameFrame(bool simulating) {
	if (!simulating) {
		return;
	}
	if (!navMeshLoadAttempted) {
		NavErrorType rc = TheNavMesh->Load();
		if (rc == NAV_OK) {
			Msg("Loaded Navigation mesh.\n");
		} else {
			Msg("Navigation mesh not loaded, reason code: %d", rc);
		}
		navMeshLoadAttempted = true;
	}
	if (TheNavMesh != nullptr) {
		TheNavMesh->Update();
	}
}

void VSPlugin::ClientActive(edict_t *pEntity) {
}

void VSPlugin::ClientPutInServer(edict_t *pEntity, char const *playername) {
}

void VSPlugin::ClientDisconnect(edict_t *pEntity) {
}

void VSPlugin::LevelShutdown(void) {
	gameeventmanager->RemoveListener(this);
	gameeventmanager1->RemoveListener(this);
}

void VSPlugin::Unload(void) {
	ConVar_Unregister();
	DisconnectTier1Libraries();
	DisconnectTier2Libraries();
}


