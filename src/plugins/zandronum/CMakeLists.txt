cmake_minimum_required(VERSION 2.8.12)

set(PLUGIN_NAME zandronum)
include(../PluginHeader.txt)

qt4_wrap_ui(ZANDRONUM_UI_FILES
	createserverdialogpages/flagspage.ui
	createserverdialogpages/votingsetupwidget.ui
)

set(ZANDRONUM_QT_FILES
	createserverdialogpages/flagspage.h
	createserverdialogpages/votingsetupwidget.h
	enginezandronumconfigbox.h
	zandronumbinaries.h
	zandronumbroadcast.h
	zandronum2dmflags.h
	zandronum3dmflags.h
	zandronumgamehost.h
	zandronumgameinfo.h
	zandronummasterclient.h
	zandronumrconprotocol.h
	zandronumserver.h
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
	${ZANDRONUM_QT_FILES}
	createserverdialogpages/flagsid.h
	createserverdialogpages/flagspagevaluecontroller2.h
	createserverdialogpages/flagspagevaluecontroller3.h
	huffman/bitreader.h
	huffman/bitwriter.h
	huffman/codec.h
	huffman/huffcodec.h
	huffman/huffman.h
	huffmanqt.h
	huffmanudpsocket.h
	zandronumengineplugin.h
	zandronumgameexefactory.h
	zandronumgamerunner.h
	zandronumgamesettings.h
	zandronumserverdmflagsparser.h
)

qt_wrap_cpp(zandronum ZANDRONUM_FILES ${ZANDRONUM_QT_FILES})

set(TRANSLATIONS_FILES
	translations/zandronum_pl_PL.ts
)
set(FILES_TO_TRANSLATE
	${ZANDRONUM_QT_FILES}
	${ZANDRONUM_UI_FILES}
	${ZANDRONUM_FILES}
)

include(../Translation.txt)

add_library(${PLUGIN_NAME} 
	MODULE 
	${ZANDRONUM_FILES} 
	${HEADER_FILES}
	${ZANDRONUM_UI_FILES}
	${QM_FILES}
)

target_link_libraries(${PLUGIN_NAME} LINK_PRIVATE wadseeker)

include(../PluginFooter.txt)
