cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (MSVC)
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
endif (MSVC)

add_subdirectory(lzma)

# If we are compiling wadseeker outside of doomseeker prepare its own project.
if(DEFINED DOOMSEEKER_SOURCE_DIR)
	include_directories(${DOOMSEEKER_SOURCE_DIR}/src ${DOOMSEEKER_SOURCE_DIR}/src/wadseeker/lzma/C)
else(DEFINED DOOMSEEKER_SOURCE_DIR)
	project(WADSEEKER)
	include_directories(${WADSEEKER_SOURCE_DIR}/lzma/C)
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
	zip/unarchive.h
	zip/un7zip.h
	zip/unzip.h
	speedcalculator.h
	wadseeker.h
	www.h
)

set(WADSEEKER_FILES
	protocols/ftp.cpp
	protocols/http.cpp
	protocols/idgames.cpp
	protocols/protocol.cpp
	zip/un7zip.cpp
	zip/unzip.cpp
	html.cpp
	link.cpp
	speedcalculator.cpp
	wadseeker.cpp
	wadseekerversioninfo.cpp
	www.cpp
)

# For the purpose of generation proper project files.
set(HEADER_FILES
	${WADSEEKER_QT_FILES}
	html.h
	link.h
	wadseekerexportinfo.h	
	wadseekerversioninfo.h	
)

include_directories(${QT_INCLUDES})

qt_wrap_cpp(wadseekerQt WADSEEKER_FILES ${WADSEEKER_QT_FILES})

add_library(wadseeker SHARED ${WADSEEKER_FILES} ${HEADER_FILES})

target_link_libraries(wadseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} lzma)

if(DEFINED DOOMSEEKER_SOURCE_DIR)
	set_target_properties(wadseeker PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR} RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endif(DEFINED DOOMSEEKER_SOURCE_DIR)

if(NOT WIN32)
	install(TARGETS wadseeker LIBRARY DESTINATION lib)
endif(NOT WIN32)