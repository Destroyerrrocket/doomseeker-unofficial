cmake_minimum_required(VERSION 2.4)

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
	huffmanqt.h
	huffmanudpsocket.h
	zandronumbinaries.h
	zandronumdmflags.h
	zandronumgamehost.h
	zandronumgameinfo.h
	zandronummasterclient.h
	zandronumrconprotocol.h
	zandronumserver.h
)

set(ZANDRONUM_FILES
	createserverdialogpages/flagspagevaluecontroller.cpp
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
	zandronumdmflags.cpp
	zandronumengineplugin.cpp
	zandronumgamehost.cpp
	zandronumgameinfo.cpp
	zandronumgamerunner.cpp
	zandronummasterclient.cpp
	zandronumrconprotocol.cpp
	zandronumserver.cpp
	zandronumserverdmflagsparser.cpp
)

set(HEADER_FILES
	${ZANDRONUM_QT_FILES}
	createserverdialogpages/flagspagevaluecontroller.h
	huffman/bitreader.h
	huffman/bitwriter.h
	huffman/codec.h
	huffman/huffcodec.h
	huffman/huffman.h
	zandronumengineplugin.h
	zandronumgamerunner.h
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

target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS} wadseeker)

include(../PluginFooter.txt)
