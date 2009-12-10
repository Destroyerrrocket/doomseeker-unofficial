cmake_minimum_required(VERSION 2.4)

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
	add_definitions(-DMODE_PLUGIN)
endif(WIN32)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
qt4_wrap_ui(chocolateDoomUi
	engineChocolateDoomConfig.ui
)
set(CHOCOLATEDOOM_QT_FILES
	chocolatedoomserver.h
	engineChocolateDoomConfig.h
)
set(CHOCOLATEDOOM_FILES
	chocolatedoommain.cpp
	chocolatedoomserver.cpp
	engineChocolateDoomConfig.cpp
)
include_directories(${QT_INCLUDES})
qt_wrap_cpp(chocolateDoomQt CHOCOLATEDOOM_FILES ${CHOCOLATEDOOM_QT_FILES})

add_library(chocolatedoom MODULE ${chocolateDoomUi} ${CHOCOLATEDOOM_FILES})
add_dependencies(chocolatedoom doomseeker)
target_link_libraries(chocolatedoom ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} doomseeker)

set_target_properties(chocolatedoom PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/engines/ RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/engines/)

if(NOT WIN32)
	install(FILES ${OUTPUT_DIR}/engines/libchocolatedoom.so DESTINATION share/doomseeker/engines)
endif(NOT WIN32)