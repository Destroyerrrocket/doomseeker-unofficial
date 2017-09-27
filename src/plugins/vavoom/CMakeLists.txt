cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME vavoom)
set(PLUGIN_VERSION 17)
include(../PluginHeader.cmake)

set(VAVOOM_FILES
	vavoomengineplugin.cpp
	vavoomgameinfo.cpp
	vavoomgamerunner.cpp
	vavoommasterclient.cpp
	vavoomserver.cpp
)

set(HEADER_FILES
	vavoomengineplugin.h
	vavoomgameinfo.h
	vavoomgamerunner.h
	vavoommasterclient.h
	vavoomserver.h
)

add_library(${PLUGIN_NAME} MODULE ${VAVOOM_FILES} ${HEADER_FILES})

include(../PluginFooter.cmake)
