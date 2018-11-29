# Valve Source Navigation Mesh for Plugins
This is a refactor of the Valve Source navigation mesh code from [Valve's SDK](https://github.com/alliedmodders/hl2sdk) to allow for it to be used on a Valve Source Plugin. This allows a developer to create bots against mods or games that do not have bot support without having to create a new navigation system.  

A sample plugin is provided with the code base as an example of how to build and use the nav mesh with a Valve Source Plugin.  Building the sample requires [CMake](https://cmake.org/) and a copy of Valve's Source SDK.  While the compilation of the test plugin should work against all supported platforms, it was only tested against LINUX.

The functionally is unchanged from that of sdk2013.  Most of the changes made were to reduce the size of the code base.  Code blocks commented out with TODO are from the original that either is impossible to port or cannot be ported witout signficant effort.  The plugin navmesh should be able to read .nav files generated by mods that already support Valve nav meshes (like TF2 or CSS), however any mod specific attributes from the nav mesh are ignored.

Some Screenshots:

Lock Down from HL2 Death Match
![dm_lockdown0000](https://user-images.githubusercontent.com/43772004/48299281-dc221200-e47f-11e8-907b-f2b44e5976a2.jpeg)

Avalanche from DoD Source
![dod_avalanche0008](https://user-images.githubusercontent.com/43772004/48299306-59e61d80-e480-11e8-827a-82d156b10fa7.jpeg)
