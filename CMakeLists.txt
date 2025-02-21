cmake_minimum_required (VERSION 3.28.3)

project (plugin_navmesh)

set(DRIVE c CACHE STRING "User specified drive for Windows")
set( steam_root "${DRIVE}:/Program Files (x86)/steam" )
set(HL2SDK_DIR "${PROJECT_SOURCE_DIR}/../hl2sdk")
set (HL2SDK_PUB_LIBS ${HL2SDK_DIR}/lib/public)
set (HL2SDK_COM_LIBS ${HL2SDK_DIR}/lib/common)
set (mods "tf;dod;hl2mp")
set(shared_defs  PLATFORM_64BITS X64BITS RAD_TELEMETRY_DISABLED $<$<CONFIG:Debug>:_DEBUG>)

set (tier1 tier1)
set (mathlib mathlib)
set (lzma lzma)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	list(APPEND shared_defs __clang__)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	list(APPEND shared_defs GNUC)
endif()

if (LINUX)
	set (tier1 :tier1.a)
	set (mathlib :mathlib.a)
	set (lzma :liblzma.a)
	set (HL2SDK_COM_LIBS ${HL2SDK_COM_LIBS}/inux64)
	set (HL2SDK_PUB_LIBS ${HL2SDK_PUB_LIBS}/linux64)
	set (SRC_HOME $ENV{HOME})
	set( steam_root "$ENV{HOME}/.steam/steam" )
	set(my_compile_options $<$<CONFIG:Debug>:-Wall> -fPIC -m64)
	list(APPEND shared_defs _LINUX LINUX POSIX)
	set(my_link_options -static-libgcc -static-libstdc++)
else()
	set (HL2SDK_PUB_LIBS ${HL2SDK_PUB_LIBS}/x64)
	set (HL2SDK_COM_LIBS ${HL2SDK_COM_LIBS}/x64/2015/release)
	set(my_compile_options
		$<$<CONFIG:>:/MT> #---------|
		$<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
		$<$<CONFIG:Release>:/MT> #--|
		-GS- -TP -W4 -fp:fast -GF -Zc:forScope -RTC1 -GR -Gd -Oy- -Fa -Fo -Fp -MP -Od -TP
		)
	list(APPEND shared_defs COMPILER_MSVC COMPILER_MSVC64 _CRT_NO_VA_START_VALIDATION _CRT_SECURE_NO_DEPRECATE 
		_CRT_NONSTDC_NO_DEPRECATE)
	set(my_link_options /NODEFAULTLIB:libcmtd /SAFESEH:NO)
endif ()

set(shared_includes ${HL2SDK_DIR}/public ${HL2SDK_DIR}/public/tier0 
	${HL2SDK_DIR}/public/engine ${HL2SDK_DIR}/public/tier1 ${HL2SDK_DIR}/public/mathlib
	${HL2SDK_DIR}/public/game/server  
	${HL2SDK_DIR}/game/shared ${HL2SDK_DIR}/game/server)
add_subdirectory(${PROJECT_SOURCE_DIR}/util)
add_subdirectory(${PROJECT_SOURCE_DIR}/nav_mesh)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)
