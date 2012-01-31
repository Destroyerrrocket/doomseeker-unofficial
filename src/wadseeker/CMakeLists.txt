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

find_package(Qt4 COMPONENTS QtCore QtNetwork REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

include(${QT_USE_FILE})
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}	
	${ZLIB_INCLUDE_DIR} 
	${BZIP2_INCLUDE_DIR}
)

set(WADSEEKER_QT_FILES
	protocols/fixednetworkaccessmanager.h
	protocols/fixedftpreply.h
	protocols/networkreplysignalwrapper.h
	wadretriever/wadretriever.h
	wwwseeker/idgames.h
	wwwseeker/wwwseeker.h
	zip/unarchive.h
	zip/un7zip.h
	zip/untar.h
	zip/unzip.h
	wadseeker.h
)

set(WADSEEKER_FILES
	entities/link.cpp
	entities/waddownloadinfo.cpp
	protocols/entities/networkreplywrapperinfo.cpp
	protocols/fixednetworkaccessmanager.cpp
	protocols/fixedftpreply.cpp
	protocols/http.cpp
	protocols/networkreplysignalwrapper.cpp
	wadretriever/wadinstaller.cpp
	wadretriever/wadretriever.cpp
	wwwseeker/entities/fileseekinfo.cpp
	wwwseeker/htmlparser.cpp
	wwwseeker/idgames.cpp
	wwwseeker/urlparser.cpp
	wwwseeker/wwwseeker.cpp
	zip/unarchive.cpp
	zip/un7zip.cpp
	zip/untar.cpp
	zip/unzip.cpp
	ioutils.cpp
	wadseeker.cpp
	wadseekerversioninfo.cpp
)

# For the purpose of generation proper project files.
set(HEADER_FILES
	${WADSEEKER_QT_FILES}
	entities/link.h
	entities/waddownloadinfo.h
	protocols/entities/networkreplywrapperinfo.h
	protocols/http.h
	wadretriever/wadinstaller.h
	wwwseeker/entities/fileseekinfo.h
	wwwseeker/htmlparser.h
	wwwseeker/urlparser.h
	ioutils.h
	wadseekerexportinfo.h	
	wadseekermessagetype.h
	wadseekerversioninfo.h	
)

include_directories(${QT_INCLUDES})

qt_wrap_cpp(wadseekerQt WADSEEKER_FILES ${WADSEEKER_QT_FILES})

add_library(wadseeker SHARED ${WADSEEKER_FILES} ${HEADER_FILES})

target_link_libraries(wadseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} lzma)

if(DEFINED DOOMSEEKER_SOURCE_DIR)
	set_target_properties(wadseeker PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR} RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endif(DEFINED DOOMSEEKER_SOURCE_DIR)

if(NOT WIN32 AND NOT XCODE_VERSION)
	install(TARGETS wadseeker LIBRARY DESTINATION lib)
endif(NOT WIN32 AND NOT XCODE_VERSION)
