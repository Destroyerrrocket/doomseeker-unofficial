cmake_minimum_required(VERSION 2.4)

set(PLUGIN_NAME chocolatedoom)
include(../PluginHeader.txt)

qt4_wrap_ui(chocolateDoomUi
	engineChocolateDoomConfig.ui
)
set(CHOCOLATEDOOM_QT_FILES
	chocolatedoomserver.h
	engineChocolateDoomConfig.h
)
set(CHOCOLATEDOOM_FILES
	chocolatedoomgamerunner.cpp
	chocolatedoommain.cpp
	chocolatedoomserver.cpp
	engineChocolateDoomConfig.cpp
)

set(HEADER_FILES
	${CHOCOLATEDOOM_QT_FILES}
	chocolatedoombinaries.h
	chocolatedoomgamerunner.h
	chocolatedoommain.h
)

qt_wrap_cpp(chocolateDoom CHOCOLATEDOOM_FILES ${CHOCOLATEDOOM_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${chocolateDoomUi} ${CHOCOLATEDOOM_FILES} ${HEADER_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

include(../PluginFooter.txt)
