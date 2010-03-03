cmake_minimum_required(VERSION 2.4)

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
	add_definitions(-DMODE_PLUGIN)
endif(WIN32)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
qt4_wrap_ui(vavoomUi
	engineVavoomConfig.ui
)
set(VAVOOM_QT_FILES
	engineVavoomConfig.h
	vavoommasterclient.h
	vavoomserver.h
)
set(VAVOOM_FILES
	engineVavoomConfig.cpp
	vavoomgameinfo.cpp
	vavoomgamerunner.cpp
	vavoommain.cpp
	vavoommasterclient.cpp
	vavoomserver.cpp
)
include_directories(${QT_INCLUDES})
qt_wrap_cpp(vavoomQt VAVOOM_FILES ${VAVOOM_QT_FILES})

add_library(vavoom MODULE ${vavoomUi} ${VAVOOM_FILES})
add_dependencies(vavoom doomseeker)
target_link_libraries(vavoom ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} doomseeker)

set_target_properties(vavoom PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/engines/ RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/engines/)

if(NOT WIN32)
	install(FILES ${OUTPUT_DIR}/engines/libvavoom.so DESTINATION share/doomseeker/engines)
endif(NOT WIN32)