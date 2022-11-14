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
else ()
	add_definitions(-DWIN32 -D_WIN32 -DWINDOWS)
endif (${UNIX})

#compiler specific settings
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		add_definitions(-DCOMPILER_GCC -DGNUC)
	else()
		add_definitions(-D__clang__)
	endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_compile_options(-march=pentium3 -mmmx -m32 $<$<CONFIG:Debug>:-Wall>)
	add_definitions(-D_POSIX -DPOSIX -DGNUC -DNO_HOOK_MALLOC -DNO_MALLOC_OVERRIDE)
	set(my_link_options -static-libgcc -static-libstdc++ -m32)
else() #MSVC
	add_compile_options(
		$<$<CONFIG:>:/MT> #---------|
		$<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
		$<$<CONFIG:Release>:/MT> #--|
		-arch:SSE -GS- -TP -W4 -fp:fast -GF -Zc:forScope -RTC1 -GR -Gd -Oy- -Fa -Fo -Fp -MP -Od -TP
	)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	set(my_link_options /NODEFAULTLIB:libcmtd /SAFESEH:NO)
	add_definitions(-DCOMPILER_MSVC -DCOMPILER_MSVC32 -D_CRT_NO_VA_START_VALIDATION -D_CRT_SECURE_NO_DEPRECATE 
		-D_CRT_NONSTDC_NO_DEPRECATE)
	set (LEGACY_STDIO legacy_stdio_definitions)
endif ()

set(shared_includes ${HL2SDK_DIR}/public ${HL2SDK_DIR}/public/tier0 
	${HL2SDK_DIR}/public/engine ${HL2SDK_DIR}/public/tier1 ${HL2SDK_DIR}/public/mathlib
	${HL2SDK_DIR}/public/game/server  
	${HL2SDK_DIR}/game/shared ${HL2SDK_DIR}/game/server)
add_subdirectory(${PROJECT_SOURCE_DIR}/util)
target_include_directories(plugin_util PUBLIC ${shared_includes})
add_subdirectory(${PROJECT_SOURCE_DIR}/nav_mesh)
target_include_directories(valve_navmesh PUBLIC ${shared_includes} ${CMAKE_SOURCE_DIR} )
add_subdirectory(${PROJECT_SOURCE_DIR}/test)
