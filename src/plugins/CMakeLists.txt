add_subdirectory(chocolate-doom)
add_subdirectory(odamex)
add_subdirectory(srb2)
add_subdirectory(zandronum)

# For unmaintained source ports
option(BUILD_LEGACY_PLUGINS "Build plugins for discontinued source ports." OFF)
if(BUILD_LEGACY_PLUGINS)
	add_subdirectory(skulltag)
	add_subdirectory(vavoom)
	add_subdirectory(zdaemon)
endif()

option(BUILD_FAKE_PLUGINS "Build plugins that are used for development purposes only." OFF)
if(BUILD_FAKE_PLUGINS)
	add_subdirectory(fakeplugin)
endif()
