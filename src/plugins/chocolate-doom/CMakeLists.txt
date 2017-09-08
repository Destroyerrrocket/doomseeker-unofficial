cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME chocolatedoom)
include(../PluginHeader.cmake)

set(CHOCOLATEDOOM_FILES
	chocolatedoomengineplugin.cpp
	chocolatedoomgameexefactory.cpp
	chocolatedoomgamehost.cpp
	chocolatedoomgameinfo.cpp
	chocolatedoomgamerunner.cpp
	chocolatedoommasterclient.cpp
	chocolatedoomserver.cpp
)

set(HEADER_FILES
	chocolatedoomengineplugin.h
	chocolatedoomgamehost.h
	chocolatedoomgameexefactory.h
	chocolatedoomgamerunner.h
	chocolatedoommasterclient.h
	chocolatedoomserver.h
)

add_library(${PLUGIN_NAME} MODULE ${CHOCOLATEDOOM_FILES} ${HEADER_FILES})

include(../PluginFooter.cmake)
