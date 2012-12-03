add_subdirectory(chocolate-doom)
add_subdirectory(odamex)
add_subdirectory(vavoom)
add_subdirectory(zandronum)

# For unmaintained source ports
option(BUILD_LEGACY_PLUGINS "Build plugins for discontinued source ports." OFF)
if(BUILD_LEGACY_PLUGINS)
	add_subdirectory(skulltag)
	add_subdirectory(zdaemon)
endif(BUILD_LEGACY_PLUGINS)
