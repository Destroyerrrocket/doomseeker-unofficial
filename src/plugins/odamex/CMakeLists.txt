cmake_minimum_required(VERSION 2.4)

set(PLUGIN_NAME odamex)
include(../PluginHeader.txt)

set(ODAMEX_QT_FILES
	odamexmasterclient.h
	odamexserver.h
)
set(ODAMEX_FILES
	odamexengineplugin.cpp
	odamexgameinfo.cpp
	odamexgamerunner.cpp
	odamexmasterclient.cpp
	odamexserver.cpp
)

set(HEADER_FILES
	${ODAMEX_QT_FILES}
	odamexengineplugin.h
	odamexgameinfo.h
	odamexgamerunner.h
)

qt_wrap_cpp(odamex ODAMEX_FILES ${ODAMEX_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${ODAMEX_FILES} ${HEADER_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

include(../PluginFooter.txt)
