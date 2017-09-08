cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME fakeplugin)
set(DONT_INSTALL ON)
include(../PluginHeader.cmake)

set(FAKEPLUGIN_FILES
	responder/masterresponder.cpp
	responder/respondercfg.cpp
	responder/serverresponder.cpp
	pluginengineplugin.cpp
	plugingamehost.cpp
	pluginmasterclient.cpp
	pluginserver.cpp
)

set(HEADER_FILES
	responder/awaitingclient.h
	responder/masterresponder.h
	responder/respondercfg.h
	responder/serverresponder.h
	pluginengineplugin.h
	plugingamehost.h
	pluginserver.h
	pluginmasterclient.h
)

add_library(${PLUGIN_NAME} MODULE ${FAKEPLUGIN_FILES} ${HEADER_FILES})

include(../PluginFooter.cmake)
