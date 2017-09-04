cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME srb2)
include(../PluginHeader.txt)

set(SRB2_QT_FILES
	srb2gamehost.h
	srb2gameinfo.h
	srb2masterclient.h
	srb2server.h
)
set(SRB2_FILES
	srb2engineplugin.cpp
	srb2gameclientrunner.cpp
	srb2gamehost.cpp
	srb2gameinfo.cpp
	srb2masterclient.cpp
	srb2server.cpp
)

set(HEADER_FILES
	${SRB2_QT_FILES}
	srb2engineplugin.h
	srb2gameclientrunner.h
)

qt_wrap_cpp(srb2 SRB2_FILES ${SRB2_QT_FILES})

set(TRANSLATIONS_FILES
	translations/srb2_pl_PL.ts
)
set(FILES_TO_TRANSLATE
	${SRB2_QT_FILES}
	${SRB2_FILES}
)

include(../Translation.txt)

add_library(${PLUGIN_NAME} MODULE ${SRB2_FILES} ${HEADER_FILES} ${QM_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

include(../PluginFooter.txt)
