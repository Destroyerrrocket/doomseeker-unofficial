cmake_minimum_required(VERSION 2.4)

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
	add_definitions(-DMODE_PLUGIN)
endif(WIN32)

if (MSVC)
	set(CMAKE_SHARED_MODULE_PREFIX "lib")
endif (MSVC)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
qt4_wrap_ui(skulltagUi
	engineSkulltagConfig.ui
)
set(SKULLTAG_QT_FILES
	engineSkulltagConfig.h
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
	skulltaggameinfo.cpp
	skulltaggamerunner.cpp
	skulltagmain.cpp
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
	skulltagbinaries.h
	skulltaggameinfo.h
	skulltaggamerunner.h
	skulltagmain.h
)

include_directories(${QT_INCLUDES})
qt_wrap_cpp(skulltagQt SKULLTAG_FILES ${SKULLTAG_QT_FILES})

set(NAME skulltag)
set(LIBS_OUTPUT_DIR ${OUTPUT_DIR}/engines/)

add_library(${NAME} MODULE ${skulltagUi} ${SKULLTAG_FILES} ${HEADER_FILES})
add_dependencies(${NAME} doomseeker)
target_link_libraries(${NAME} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} doomseeker)

set_target_properties(${NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBS_OUTPUT_DIR} RUNTIME_OUTPUT_DIRECTORY ${LIBS_OUTPUT_DIR})

if(NOT WIN32)
	install(FILES ${OUTPUT_DIR}/engines/libskulltag.so DESTINATION share/doomseeker/engines)
elseif(MSVC_IDE)
	# This should fix the problem where built files are incorrectly placed into
	# <build_root>/engines/<build_type>/ . The correct path is
	# <build_root>/<build_type>/engines/
	set(DLL_NAME libskulltag.dll)
	set(Z_BUILD_TYPE \${CMAKE_INSTALL_CONFIG_NAME})
	install(FILES ${OUTPUT_DIR}/engines/${Z_BUILD_TYPE}/${DLL_NAME} DESTINATION ${OUTPUT_DIR}/${Z_BUILD_TYPE}/engines/)
endif(NOT WIN32)