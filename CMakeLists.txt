cmake_minimum_required (VERSION 2.8.12)

project (plugin_navmesh)

#OS specific settings

set(DRIVE c CACHE STRING "User specified drive for Windows")
set (SRC_HOME $ENV{HOMEPATH})
set( steam_root "${DRIVE}:/Program Files (x86)/steam" )
set(HL2SDK_DIR "${PROJECT_SOURCE_DIR}/../hl2sdk")
set (HL2SDK_LIBS ${HL2SDK_DIR}/lib/public)
set (mods "tf;dod;hl2mp")
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
set (tier1 tier1)
set (mathlib mathlib)
add_definitions(-DRAD_TELEMETRY_DISABLED)
if (${UNIX})
	set (tier1 :tier1.a)
	set (mathlib :mathlib.a)
	set (SRC_HOME $ENV{HOME})
	set (HL2SDK_LIBS ${HL2SDK_LIBS}/linux32)
	add_definitions(-D_LINUX -DLINUX) 
	set( steam_root "${SRC_HOME}/.local/share/Steam" )
	link_directories(${steam_root}/ubuntu12_32/steam-runtime/i386/usr/lib/i386-linux-gnu ${steam_root}/ubuntu12_32)
else ()
	add_definitions(-DWIN32 -D_WIN32 -DWINDOWS)
endif (${UNIX})

#compiler specific settings
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		add_definitions(-DCOMPILER_GCC -DGNUC)
	else()
		add_definitions(-D__clang__)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wno-dangling-else -Wno-c++11-narrowing -Wno-inconsistent-missing-override")
	endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set( ARCH_FLAGS "-mtune=i686 -march=pentium3 -mmmx -m32 -fpermissive" )
	add_definitions(-D_POSIX -DPOSIX -DGNUC -DNO_HOOK_MALLOC -DNO_MALLOC_OVERRIDE)
	add_definitions(-DSWDS -D_finite=finite -Dstricmp=strcasecmp  -Dstrnicmp=strncasecmp -D_vsnprintf=vsnprintf -D_alloca=alloca)
else() #MSVC
	add_compile_options(
		$<$<CONFIG:>:/MT> #---------|
		$<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
		$<$<CONFIG:Release>:/MT> #--|
		-arch:SSE -GS- -TP -W4 -fp:fast -GF -Zc:forScope -RTC1 -GR -Gd -Oy- -Fa -Fo -Fp -MP -Od -TP
	)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	add_link_options(
		/NODEFAULTLIB:libcmtd
		/SAFESEH:NO)
	add_definitions(-DCOMPILER_MSVC -DCOMPILER_MSVC32 -D_CRT_NO_VA_START_VALIDATION -D_CRT_SECURE_NO_DEPRECATE 
		-D_CRT_NONSTDC_NO_DEPRECATE)
	set (LEGACY_STDIO legacy_stdio_definitions)
endif ()
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARCH_FLAGS}" )
SET( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${ARCH_FLAGS}" )

include_directories(${PROJECT_SOURCE_DIR} ${HL2SDK_DIR}/common utils/common ${HL2SDK_DIR}/public ${HL2SDK_DIR}/public/mathlib ${HL2SDK_DIR}/public/tier0 ${HL2SDK_DIR}/public/tier1 ${HL2SDK_DIR}/public/game/server ${HL2SDK_DIR}/public/engine ${HL2SDK_DIR}/game/shared ${HL2SDK_DIR}/game/server)
link_directories("${src_dir}/bin" ${HL2SDK_LIBS})	

add_subdirectory(${PROJECT_SOURCE_DIR}/util)
add_subdirectory(${PROJECT_SOURCE_DIR}/nav_mesh)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)
