cmake_minimum_required(VERSION 2.4)

set(PLUGIN_NAME zandronum)
include(../PluginHeader.txt)

set(ZANDRONUM_QT_FILES
	enginezandronumconfigbox.h
	zandronumbinaries.h
	zandronummasterclient.h
	zandronumserver.h
)

set(ZANDRONUM_FILES
	huffman/bitreader.cpp
	huffman/bitwriter.cpp
	huffman/huffcodec.cpp
	huffman/huffman.cpp
	enginezandronumconfigbox.cpp
	zandronumbinaries.cpp
	zandronumengineplugin.cpp
	zandronumgameinfo.cpp
	zandronumgamerunner.cpp
	zandronummasterclient.cpp
	zandronumserver.cpp
)

set(HEADER_FILES
	${ZANDRONUM_QT_FILES}
	huffman/bitreader.h
	huffman/bitwriter.h
	huffman/codec.h
	huffman/huffcodec.h
	huffman/huffman.h
	zandronumengineplugin.h
	zandronumgameinfo.h
	zandronumgamerunner.h
)

qt_wrap_cpp(zandronum ZANDRONUM_FILES ${ZANDRONUM_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${ZANDRONUM_FILES} ${HEADER_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS} wadseeker)

include(../PluginFooter.txt)
