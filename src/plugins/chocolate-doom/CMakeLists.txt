cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME chocolatedoom)
include(../PluginHeader.cmake)

set(CHOCOLATEDOOM_QT_FILES
	chocolatedoomgamehost.h
	chocolatedoomserver.h
	chocolatedoommasterclient.h
)
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
	${CHOCOLATEDOOM_QT_FILES}
	chocolatedoomengineplugin.h
	chocolatedoomgameexefactory.h
	chocolatedoomgamerunner.h
)

qt_wrap_cpp(chocolateDoom CHOCOLATEDOOM_FILES ${CHOCOLATEDOOM_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${CHOCOLATEDOOM_FILES} ${HEADER_FILES})

include(../PluginFooter.cmake)
