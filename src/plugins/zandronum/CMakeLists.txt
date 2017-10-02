cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME zandronum)
set(PLUGIN_VERSION 37)
include(../PluginHeader.cmake)

qt4_wrap_ui(ZANDRONUM_UI_FILES
	createserverdialogpages/flagspage.ui
	createserverdialogpages/votingsetupwidget.ui
)

set(ZANDRONUM_FILES
	createserverdialogpages/flagsid.cpp
	createserverdialogpages/flagspagevaluecontroller2.cpp
	createserverdialogpages/flagspagevaluecontroller3.cpp
	createserverdialogpages/flagspage.cpp
	createserverdialogpages/votingsetupwidget.cpp
	huffman/bitreader.cpp
	huffman/bitwriter.cpp
	huffman/huffcodec.cpp
	huffman/huffman.cpp
	enginezandronumconfigbox.cpp
	huffmanqt.cpp
	huffmanudpsocket.cpp
	zandronumbinaries.cpp
	zandronumbroadcast.cpp
	zandronum2dmflags.cpp
	zandronum3dmflags.cpp
	zandronumengineplugin.cpp
	zandronumgameexefactory.cpp
	zandronumgamehost.cpp
	zandronumgameinfo.cpp
	zandronumgamerunner.cpp
	zandronumgamesettings.cpp
	zandronummasterclient.cpp
	zandronumrconprotocol.cpp
	zandronumserver.cpp
	zandronumserverdmflagsparser.cpp
)

set(HEADER_FILES
	createserverdialogpages/flagspage.h
	createserverdialogpages/votingsetupwidget.h
	createserverdialogpages/flagsid.h
	createserverdialogpages/flagspagevaluecontroller2.h
	createserverdialogpages/flagspagevaluecontroller3.h
	huffman/bitreader.h
	huffman/bitwriter.h
	huffman/codec.h
	huffman/huffcodec.h
	huffman/huffman.h
	enginezandronumconfigbox.h
	huffmanqt.h
	huffmanudpsocket.h
	zandronumbinaries.h
	zandronumbroadcast.h
	zandronum2dmflags.h
	zandronum3dmflags.h
	zandronumengineplugin.h
	zandronumgameexefactory.h
	zandronumgamehost.h
	zandronumgameinfo.h
	zandronumgamerunner.h
	zandronumgamesettings.h
	zandronummasterclient.h
	zandronumrconprotocol.h
	zandronumserver.h
	zandronumserverdmflagsparser.h
)

set(TRANSLATIONS_FILES
	translations/zandronum_pl_PL.ts
)
set(FILES_TO_TRANSLATE
	${HEADER_FILES}
	${ZANDRONUM_UI_FILES}
	${ZANDRONUM_FILES}
)

include(../Translation.cmake)

add_library(${PLUGIN_NAME} 
	MODULE 
	${ZANDRONUM_FILES} 
	${HEADER_FILES}
	${ZANDRONUM_UI_FILES}
	${QM_FILES}
)

if (NOT UNIX OR APPLE)
	# Linking Wadseeker is not needed on Linux and only causes
	# the resulting .so to get an RPATH. Windows still needs
	# to link it, though, or otherwise the compilation will fail.
	target_link_libraries(${PLUGIN_NAME} LINK_PRIVATE wadseeker)
endif()

include(../PluginFooter.cmake)
