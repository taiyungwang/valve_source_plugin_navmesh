#include "common_util.h"

#include <eiface.h>
#include <strtools.h>

bool isGameName(const char *name) {
	extern IVEngineServer* engine;
	char gameDir[MAX_PATH];
	engine->GetGameDir(gameDir, MAX_PATH);
	return V_strEndsWith(gameDir, name);
}
