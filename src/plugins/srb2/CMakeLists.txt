cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME srb2)
set(PLUGIN_VERSION 9)
include(../PluginHeader.cmake)

set(SRB2_FILES
	srb2engineplugin.cpp
	srb2gameclientrunner.cpp
	srb2gamehost.cpp
	srb2gameinfo.cpp
	srb2masterclient.cpp
	srb2server.cpp
)

set(HEADER_FILES
	srb2engineplugin.h
	srb2gameclientrunner.h
	srb2gamehost.h
	srb2gameinfo.h
	srb2masterclient.h
	srb2server.h
)

set(TRANSLATIONS_FILES
	translations/srb2_pl_PL.ts
)
set(FILES_TO_TRANSLATE
	${HEADER_FILES}
	${SRB2_FILES}
)

include(../Translation.cmake)

add_library(${PLUGIN_NAME} MODULE ${SRB2_FILES} ${HEADER_FILES} ${QM_FILES})

include(../PluginFooter.cmake)
