add_library(valve_navmesh nav_colors.cpp nav_area.cpp nav_node.cpp 
	nav_mesh.cpp nav_generate.cpp nav_file.cpp nav_merge.cpp nav_simplify.cpp nav_edit.cpp nav_ladder.cpp
	nav_entities.cpp)
set_property(TARGET valve_navmesh PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(valve_navmesh PUBLIC ${shared_defs})
target_include_directories(valve_navmesh PUBLIC ${shared_includes} ${CMAKE_SOURCE_DIR} )
target_compile_options(valve_navmesh PUBLIC ${my_compile_options})
