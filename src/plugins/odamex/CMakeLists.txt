cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME odamex)
include(../PluginHeader.txt)

set(ODAMEX_QT_FILES
	odamexgamehost.h
	odamexgameinfo.h
	odamexmasterclient.h
	odamexserver.h
)
set(ODAMEX_FILES
	odamexengineplugin.cpp
	odamexgamehost.cpp
	odamexgameinfo.cpp
	odamexgamerunner.cpp
	odamexmasterclient.cpp
	odamexserver.cpp
)

set(HEADER_FILES
	${ODAMEX_QT_FILES}
	odamexengineplugin.h
	odamexgamerunner.h
)

qt_wrap_cpp(odamex ODAMEX_FILES ${ODAMEX_QT_FILES})

set(TRANSLATIONS_FILES
	translations/odamex_pl_PL.ts
)
set(FILES_TO_TRANSLATE
	${ODAMEX_QT_FILES}
	${ODAMEX_FILES}
)

include(../Translation.txt)

add_library(${PLUGIN_NAME} MODULE ${ODAMEX_FILES} ${HEADER_FILES} ${QM_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

include(../PluginFooter.txt)
