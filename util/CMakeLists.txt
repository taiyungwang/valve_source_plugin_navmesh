add_library(plugin_util
 SimpleException.cpp BaseEntity.cpp UtilTrace.cpp EntityUtils.cpp Handle.cpp)
target_compile_definitions(plugin_util PUBLIC ${shared_defs})
set_property(TARGET plugin_util PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(plugin_util PUBLIC ${shared_includes})
target_compile_options(plugin_util PUBLIC ${my_compile_options})