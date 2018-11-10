# valve_source_plugin_navmesh
This is a refactor of the Valve Source navigation mesh code from Valve's SDK (sdk2013) to allow for it to be used on a Valve Source Plugin. This allows a developer to create bots against mods or games that do not have bot support without having to create a new navigation system.  

A sample plugin is provided as an example of how to build and use the nav_mesh with a Valve Source Plugin.  Building requires CMake.  While the compilation of the test plugin should work against all supported platforms, it was only really tested against LINUX.

The functionally is largely unchanged from that of sdk2013.  Most of the changes made were to reduce the size of the code base.  Code blocks commented out with TODO are from the original that either is impossible to port or cannot be ported witout signficant effort.  The plugin navmesh should be able to read .nav files generated by mods that already support Valve nav meshes (like TF2 or CSS), however any customized attributes specific to those mods will not be available.

