cmake_minimum_required(VERSION 2.4)

# If we are compiling wadseeker outside of doomseeker prepare its own project.
if(DEFINED DOOMSEEKER_SOURCE_DIR)
	include_directories(${DOOMSEEKER_SOURCE_DIR}/src)
else(DEFINED DOOMSEEKER_SOURCE_DIR)
	project(WADSEEKER)
endif(DEFINED DOOMSEEKER_SOURCE_DIR)

if(WIN32)
	add_definitions(-DWADSEEKER_API_EXPORT)
endif(WIN32)

find_package(Qt4 REQUIRED)
find_package(ZLIB)

include(${QT_USE_FILE})
include_directories(${ZLIB_INCLUDE_DIR})
set(WADSEEKER_QT_FILES
  protocols/ftp.h
  protocols/http.h
  protocols/idgames.h
  protocols/protocol.h
  zip/unzip.h
	wadseeker.h
  www.h
)

set(WADSEEKER_FILES
  protocols/ftp.cpp
  protocols/http.cpp
	protocols/idgames.cpp
  protocols/protocol.cpp
  zip/unzip.cpp
	html.cpp
	link.cpp
  wadseeker.cpp
  www.cpp
)

include_directories(${QT_INCLUDES})

qt_wrap_cpp(wadseekerQt WADSEEKER_FILES ${WADSEEKER_QT_FILES})

add_library(wadseeker SHARED ${WADSEEKER_FILES})

target_link_libraries(wadseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})

if(DEFINED DOOMSEEKER_SOURCE_DIR)
	set_target_properties(wadseeker PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR} RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endif(DEFINED DOOMSEEKER_SOURCE_DIR)

