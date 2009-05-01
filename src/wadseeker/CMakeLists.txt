cmake_minimum_required(VERSION 2.4)

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)

if(WIN32)
	add_definitions(-DMODE_PLUGIN)
endif(WIN32)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
set(WADSEEKER_QT_FILES
	http.h
	wadseeker.h
)
set(WADSEEKER_FILES
  http.cpp
  wadseeker.cpp
)

include_directories(${QT_INCLUDES})

qt_wrap_cpp(wadseekerQt WADSEEKER_FILES ${WADSEEKER_QT_FILES})

add_library(wadseeker SHARED ${WADSEEKER_FILES})

target_link_libraries(wadseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})

set_target_properties(wadseeker PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR} RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

