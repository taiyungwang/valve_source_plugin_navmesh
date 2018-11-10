cmake_minimum_required (VERSION 2.8.7)

project (plugin_navmesh)

#OS specific settings
add_definitions(-DRAD_TELEMETRY_DISABLED)

set (SRC_HOME $ENV{HOMEPATH})
set( steam_root "c:/Program Files (x86)/steam" )
set(HL2SDK_DIR "../hl2sdk")
set (HL2SDK_LIBS ${HL2SDK_DIR}/lib/public)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif()
if (${UNIX})
	set (SRC_HOME $ENV{HOME})
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
	if( ${APPLE} )
		set (HL2SDK_LIBS ${HL2SDK_LIBS}/osx32)
		add_definitions(-D_OSX -DOSX)
		set( steam_root "${SRC_HOME}/Library/Application Support" )
	else ()
		set (HL2SDK_LIBS ${HL2SDK_LIBS}/linux32)
		add_definitions(-D_LINUX -DLINUX) 
		set( steam_root "${SRC_HOME}/.local/share/Steam" )
	endif( ${APPLE} )
	link_directories(${steam_root}/ubuntu12_32/steam-runtime/i386/usr/lib/i386-linux-gnu ${steam_root}/ubuntu12_32)
	set( UNDEF "-Usprintf -Ustrncpy -UPROTECTED_THINGS_ENABLE" )
else ()
	set (LEGACY_STDIO legacy_stdio_definitions)
	add_definitions(-DWIN32 -D_WIN32 -DWINDOWS)
endif (${UNIX})

#compiler specific settings
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		add_definitions(-DCOMPILER_GCC -DGNUC)
	else()
		add_definitions(-D__clang__ )
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-dangling-else -Wno-c++11-narrowing -Wno-inconsistent-missing-override")
	endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set( ARCH_FLAGS "-mtune=i686 -march=pentium3 -mmmx -m32 -fpermissive" )
	add_definitions(-D_POSIX -DPOSIX -DGNUC -DNO_HOOK_MALLOC -DNO_MALLOC_OVERRIDE)
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-virtual-dtor -Wno-invalid-offsetof" )
	add_definitions(-DSWDS -D_finite=finite -Dstricmp=strcasecmp  -Dstrnicmp=strncasecmp -D_vsnprintf=vsnprintf -D_alloca=alloca)	
endif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARCH_FLAGS}" )
SET( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${ARCH_FLAGS}" )

include_directories(${PROJECT_SOURCE_DIR} ${HL2SDK_DIR}/common utils/common ${HL2SDK_DIR}/public ${HL2SDK_DIR}/public/mathlib ${HL2SDK_DIR}/public/tier0 ${HL2SDK_DIR}/public/tier1 ${HL2SDK_DIR}/public/game/server ${HL2SDK_DIR}/public/engine ${HL2SDK_DIR}/game/shared ${HL2SDK_DIR}/game/server)

#find installed mods on system.
set (mods "tf;dod;hl2mp")
set( steam_dir steamapps/common )
FILE(GLOB games ${steam_root}/${steam_dir}/*)
foreach(GAME ${games})
	foreach(mod ${mods})
		if (EXISTS ${GAME}/${mod})
			set( src_dir "${GAME}" )
			list(APPEND mod_dirs "${GAME}/${mod}")
		endif (EXISTS ${GAME}/${mod})
	endforeach(mod ${mods})
endforeach(GAME ${games})
link_directories("${src_dir}/bin" ${HL2SDK_LIBS})	
if (NOT DEFINED src_dir)
    message ("No supported mods found.")
    return()
endif (NOT DEFINED src_dir)

set(PLUGIN_NAME plugin_navmesh)


add_library(tier1 STATIC ${HL2SDK_DIR}/tier1/bitbuf.cpp ${HL2SDK_DIR}/tier1/datamanager.cpp  ${HL2SDK_DIR}/tier1/lzmaDecoder.cpp             ${HL2SDK_DIR}/tier1/qsort_s.cpp                ${HL2SDK_DIR}/tier1/stringpool.cpp        ${HL2SDK_DIR}/tier1/utlbuffer.cpp
	${HL2SDK_DIR}/tier1/byteswap.cpp       ${HL2SDK_DIR}/tier1/diff.cpp         ${HL2SDK_DIR}/tier1/mempool.cpp                 ${HL2SDK_DIR}/tier1/rangecheckedvar.cpp        ${HL2SDK_DIR}/tier1/strtools.cpp         ${HL2SDK_DIR}/tier1/utlbufferutil.cpp
	${HL2SDK_DIR}/tier1/characterset.cpp   ${HL2SDK_DIR}/tier1/fileio.cpp       ${HL2SDK_DIR}/tier1/memstack.cpp                ${HL2SDK_DIR}/tier1/reliabletimer.cpp          ${HL2SDK_DIR}/tier1/strtools_unicode.cpp  ${HL2SDK_DIR}/tier1/utlstring.cpp
	${HL2SDK_DIR}/tier1/checksum_crc.cpp   ${HL2SDK_DIR}/tier1/generichash.cpp  ${HL2SDK_DIR}/tier1/NetAdr.cpp                  ${HL2SDK_DIR}/tier1/snappy.cpp                 ${HL2SDK_DIR}/tier1/tier1.cpp             ${HL2SDK_DIR}/tier1/utlsymbol.cpp
	${HL2SDK_DIR}/tier1/checksum_md5.cpp   ${HL2SDK_DIR}/tier1/ilocalize.cpp    ${HL2SDK_DIR}/tier1/newbitbuf.cpp               ${HL2SDK_DIR}/tier1/snappy-sinksource.cpp      ${HL2SDK_DIR}/tier1/tokenreader.cpp
	${HL2SDK_DIR}/tier1/checksum_sha1.cpp  ${HL2SDK_DIR}/tier1/interface.cpp    ${HL2SDK_DIR}/tier1/pathmatch.cpp               ${HL2SDK_DIR}/tier1/snappy-stubs-internal.cpp  ${HL2SDK_DIR}/tier1/undiff.cpp
	${HL2SDK_DIR}/tier1/commandbuffer.cpp  ${HL2SDK_DIR}/tier1/KeyValues.cpp    ${HL2SDK_DIR}/tier1/sparsematrix.cpp           ${HL2SDK_DIR}/tier1/uniqueid.cpp
	${HL2SDK_DIR}/tier1/convar.cpp         ${HL2SDK_DIR}/tier1/kvpacker.cpp     ${HL2SDK_DIR}/tier1/splitstring.cpp            ${HL2SDK_DIR}/tier1/utlbinaryblock.cpp
)

add_library(mathlib ${HL2SDK_DIR}/mathlib/color_conversion.cpp  ${HL2SDK_DIR}/mathlib/lightdesc.cpp     ${HL2SDK_DIR}/mathlib/quantize.cpp                  ${HL2SDK_DIR}/mathlib/spherical.cpp  
	${HL2SDK_DIR}/mathlib/almostequal.cpp  ${HL2SDK_DIR}/mathlib/halton.cpp            ${HL2SDK_DIR}/mathlib/mathlib_base.cpp  ${HL2SDK_DIR}/mathlib/randsse.cpp                   ${HL2SDK_DIR}/mathlib/sseconst.cpp   ${HL2SDK_DIR}/mathlib/vmatrix.cpp
	${HL2SDK_DIR}/mathlib/anorms.cpp       ${HL2SDK_DIR}/mathlib/IceKey.cpp            ${HL2SDK_DIR}/mathlib/polyhedron.cpp    ${HL2SDK_DIR}/mathlib/simdvectormatrix.cpp ${HL2SDK_DIR}/mathlib/sse.cpp  ${HL2SDK_DIR}/mathlib/3dnow.cpp
	${HL2SDK_DIR}/mathlib/bumpvects.cpp    ${HL2SDK_DIR}/mathlib/imagequant.cpp        ${HL2SDK_DIR}/mathlib/powsse.cpp        ${HL2SDK_DIR}/mathlib/sparse_convolution_noise.cpp  ${HL2SDK_DIR}/mathlib/ssenoise.cpp
)

add_library(public STATIC ${HL2SDK_DIR}/public/collisionutils.cpp)

add_subdirectory(${PROJECT_SOURCE_DIR}/util)
add_subdirectory(${PROJECT_SOURCE_DIR}/nav_mesh)

find_file(tier2 tier2${CMAKE_STATIC_LIBRARY_SUFFIX} ${HL2SDK_LIBS})
add_library(${PLUGIN_NAME} SHARED  VSPlugin.cpp)
target_link_libraries(${PLUGIN_NAME}  
	 valve_navmesh plugin_util 
	 tier1 mathlib vstdlib public tier0 ${tier2} ${LEGACY_STDIO})

foreach(mod_dir ${mod_dirs})
	if (EXISTS ${mod_dir}/addons)
	    set( install_dir ${mod_dir}/addons )
        install (TARGETS ${PLUGIN_NAME} DESTINATION ${install_dir})
	endif (EXISTS ${mod_dir}/addons)
endforeach(mod_dir ${mod_dirs})
