cmake_minimum_required(VERSION 2.4)

set(PLUGIN_NAME zdaemon)
include(../PluginHeader.txt)

set(ZDAEMON_QT_FILES
	zdaemonmasterclient.h
	zdaemonserver.h
)

set(ZDAEMON_FILES
	zdaemonengineplugin.cpp
	zdaemongameinfo.cpp
	zdaemongamerunner.cpp
	zdaemonmasterclient.cpp
	zdaemonserver.cpp
)

set(HEADER_FILES
	${ZDAEMON_QT_FILES}
	zdaemonengineplugin.h
	zdaemongameinfo.h
	zdaemongamerunner.h
)

qt_wrap_cpp(zdaemonQt ZDAEMON_FILES ${ZDAEMON_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${ZDAEMON_FILES} ${HEADER_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

include(../PluginFooter.txt)
