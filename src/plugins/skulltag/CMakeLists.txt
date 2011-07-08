cmake_minimum_required(VERSION 2.4)

set(PLUGIN_NAME skulltag)
include(../PluginHeader.txt)

set(SKULLTAG_QT_FILES
	engineSkulltagConfig.h
	skulltagbinaries.h
	skulltagmasterclient.h
	skulltagserver.h
)

set(SKULLTAG_FILES
	huffman/bitreader.cpp
	huffman/bitwriter.cpp
	huffman/huffcodec.cpp
	huffman/huffman.cpp
	engineSkulltagConfig.cpp
	skulltagbinaries.cpp
	skulltagengineplugin.cpp
	skulltaggameinfo.cpp
	skulltaggamerunner.cpp
	skulltagmasterclient.cpp
	skulltagserver.cpp
)

set(HEADER_FILES
	${SKULLTAG_QT_FILES}
	huffman/bitreader.h
	huffman/bitwriter.h
	huffman/codec.h
	huffman/huffcodec.h
	huffman/huffman.h
	skulltagengineplugin.h
	skulltaggameinfo.h
	skulltaggamerunner.h
)

qt_wrap_cpp(skulltag SKULLTAG_FILES ${SKULLTAG_QT_FILES})

add_library(${PLUGIN_NAME} MODULE ${SKULLTAG_FILES} ${HEADER_FILES})
target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS} wadseeker)

include(../PluginFooter.txt)
