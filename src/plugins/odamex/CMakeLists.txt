cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME odamex)
set(PLUGIN_VERSION 30)
include(../PluginHeader.cmake)

set(ODAMEX_FILES
	odamexengineplugin.cpp
	odamexgamehost.cpp
	odamexgameinfo.cpp
	odamexgamerunner.cpp
	odamexmasterclient.cpp
	odamexserver.cpp
)

set(HEADER_FILES
	odamexengineplugin.h
	odamexmasterclient.h
	odamexgamehost.h
	odamexgameinfo.h
	odamexgamerunner.h
	odamexserver.h
)

set(TRANSLATIONS_FILES
	translations/odamex_pl_PL.ts
)
set(FILES_TO_TRANSLATE
	${ODAMEX_HEADER_FILES}
	${ODAMEX_FILES}
)

include(../Translation.cmake)

add_library(${PLUGIN_NAME} MODULE ${ODAMEX_FILES} ${HEADER_FILES} ${QM_FILES})

include(../PluginFooter.cmake)
