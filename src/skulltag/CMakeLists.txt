cmake_minimum_required(VERSION 2.4)

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)

if(WIN32)
	add_definitions(-DMODE_PLUGIN)
endif(WIN32)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
set(SKULLTAG_QT_FILES
	skulltagmasterclient.h
	skulltagserver.h
)
set(SKULLTAG_FILES
  ../huffman/huffman.cpp
	skulltagmain.cpp
	skulltagmasterclient.cpp
	skulltagserver.cpp
)
include_directories(${QT_INCLUDES})
qt_wrap_cpp(skulltagQt SKULLTAG_FILES ${SKULLTAG_QT_FILES})

add_library(skulltag SHARED ${SKULLTAG_FILES})
add_dependencies(skulltag doomseeker)
target_link_libraries(skulltag ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} doomseeker)

set_target_properties(skulltag PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/engines/)
