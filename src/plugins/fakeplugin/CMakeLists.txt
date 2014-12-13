cmake_minimum_required(VERSION 2.4)

set(PLUGIN_NAME fakeplugin)
set(DONT_INSTALL ON)
include(../PluginHeader.txt)

set(FAKEPLUGIN_QT_FILES
	responder/masterresponder.h
	responder/serverresponder.h
	pluginserver.h
	pluginmasterclient.h
)
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
	${FAKEPLUGIN_QT_FILES}
	responder/awaitingclient.h
	responder/respondercfg.h
	pluginengineplugin.h
	plugingamehost.h
)

qt_wrap_cpp(fakePlugin FAKEPLUGIN_FILES ${FAKEPLUGIN_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${FAKEPLUGIN_FILES} ${HEADER_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

include(../PluginFooter.txt)
