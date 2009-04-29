cmake_minimum_required(VERSION 2.4)

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)

if(WIN32)
	add_definitions(-DMODE_PLUGIN)
endif(WIN32)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
set(ODAMEX_QT_FILES
	odamexmasterclient.h
)
set(ODAMEX_FILES
	odamexmain.cpp
	odamexmasterclient.cpp
	odamexserver.cpp
)
include_directories(${QT_INCLUDES})
qt_wrap_cpp(odamexQt ODAMEX_FILES ${ODAMEX_QT_FILES})

add_library(odamex SHARED ${ODAMEX_FILES})
add_dependencies(odamex doomseeker)
target_link_libraries(odamex ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} doomseeker)

set_target_properties(odamex PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/engines/ RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/engines/)
