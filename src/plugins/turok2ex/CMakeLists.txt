cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME turok2ex)
include(../PluginHeader.txt)

set(TUROK2EX_QT_FILES
	turok2exgamehost.h
	turok2exgameinfo.h
	turok2exmasterclient.h
	turok2exserver.h
)
set(TUROK2EX_FILES
	turok2exengineplugin.cpp
	turok2exgamehost.cpp
	turok2exgameinfo.cpp
	turok2exgamerunner.cpp
	turok2exmasterclient.cpp
	turok2exserver.cpp
	crc.cpp
	crc.h
)

set(HEADER_FILES
	${TUROK2EX_QT_FILES}
	turok2exengineplugin.h
	turok2exgamerunner.h
)

qt_wrap_cpp(turok2ex TUROK2EX_FILES ${TUROK2EX_QT_FILES})

set(TRANSLATIONS_FILES
	translations/turok2ex_pl_PL.ts
)
set(FILES_TO_TRANSLATE
	${TUROK2EX_QT_FILES}
	${TUROK2EX_FILES}
)

include(../Translation.txt)

add_library(${PLUGIN_NAME} MODULE ${TUROK2EX_FILES} ${HEADER_FILES} ${QM_FILES})
set_property(TARGET ${PLUGIN_NAME} PROPERTY CXX_STANDARD 11)

include(../PluginFooter.txt)
