cmake_minimum_required(VERSION 2.4)

set(PLUGIN_NAME vavoom)
include(../PluginHeader.txt)

qt4_wrap_ui(vavoomUi
	engineVavoomConfig.ui
)
set(VAVOOM_QT_FILES
	engineVavoomConfig.h
	vavoommasterclient.h
	vavoomserver.h
)

set(VAVOOM_FILES
	engineVavoomConfig.cpp
	vavoomgameinfo.cpp
	vavoomgamerunner.cpp
	vavoommain.cpp
	vavoommasterclient.cpp
	vavoomserver.cpp
)

set(HEADER_FILES
	${VAVOOM_QT_FILES}
	vavoomgameinfo.h
	vavoomgamerunner.h
	vavoommain.h
)

qt_wrap_cpp(vavoomQt VAVOOM_FILES ${VAVOOM_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${vavoomUi} ${VAVOOM_FILES} ${HEADER_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

include(../PluginFooter.txt)
