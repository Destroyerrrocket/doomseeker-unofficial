cmake_minimum_required(VERSION 2.4)

set(PLUGIN_NAME chocolatedoom)
include(../PluginHeader.txt)

set(CHOCOLATEDOOM_QT_FILES
	chocolatedoomgamehost.h
	chocolatedoomserver.h
	chocolatedoommasterclient.h
)
set(CHOCOLATEDOOM_FILES
	chocolatedoomengineplugin.cpp
	chocolatedoomgamehost.cpp
	chocolatedoomgameinfo.cpp
	chocolatedoomgamerunner.cpp
	chocolatedoommasterclient.cpp
	chocolatedoomserver.cpp
)

set(HEADER_FILES
	${CHOCOLATEDOOM_QT_FILES}
	chocolatedoomengineplugin.h
	chocolatedoomgamerunner.h
)

qt_wrap_cpp(chocolateDoom CHOCOLATEDOOM_FILES ${CHOCOLATEDOOM_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${CHOCOLATEDOOM_FILES} ${HEADER_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

include(../PluginFooter.txt)
