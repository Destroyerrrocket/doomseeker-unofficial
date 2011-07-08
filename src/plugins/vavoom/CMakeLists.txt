cmake_minimum_required(VERSION 2.4)

set(PLUGIN_NAME vavoom)
include(../PluginHeader.txt)

set(VAVOOM_QT_FILES
	vavoommasterclient.h
	vavoomserver.h
)

set(VAVOOM_FILES
	vavoomengineplugin.cpp
	vavoomgameinfo.cpp
	vavoomgamerunner.cpp
	vavoommasterclient.cpp
	vavoomserver.cpp
)

set(HEADER_FILES
	${VAVOOM_QT_FILES}
	vavoomengineplugin.h
	vavoomgameinfo.h
	vavoomgamerunner.h
)

qt_wrap_cpp(vavoomQt VAVOOM_FILES ${VAVOOM_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${VAVOOM_FILES} ${HEADER_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

include(../PluginFooter.txt)
