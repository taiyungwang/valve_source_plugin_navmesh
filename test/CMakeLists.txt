set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(${PLUGIN_NAME} SHARED  VSPlugin.cpp)
target_link_libraries(${PLUGIN_NAME}  
	 valve_navmesh plugin_util 
	 tier1 :mathlib.a vstdlib public tier0 :tier2.a ${LEGACY_STDIO})

foreach(mod_dir ${mod_dirs})
	if (EXISTS ${mod_dir}/addons)
	    set( install_dir ${mod_dir}/addons )
        install (TARGETS ${PLUGIN_NAME} DESTINATION ${install_dir})
	endif (EXISTS ${mod_dir}/addons)
endforeach(mod_dir ${mod_dirs})