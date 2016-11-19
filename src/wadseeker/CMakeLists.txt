cmake_minimum_required(VERSION 2.8.7)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0020 OLD) # This is a DLL, no need for main() of any kind.
endif()

if (MSVC)
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
endif ()

# If we are compiling wadseeker outside of doomseeker prepare its own project.
if(DEFINED DOOMSEEKER_SOURCE_DIR)
	include_directories(${DOOMSEEKER_SOURCE_DIR}/src ${DOOMSEEKER_SOURCE_DIR}/src/wadseeker/lzma/C)
else()
	project(WADSEEKER)
	include_directories(${WADSEEKER_SOURCE_DIR}/lzma/C)
endif()

add_subdirectory(lzma)

if(WIN32)
	add_definitions(-DWADSEEKER_API_EXPORT)
endif()

if(NOT FORCE_QT4)
	find_package(Qt5Core)
endif()

if(NOT Qt5Core_FOUND)
	find_package(Qt4 COMPONENTS QtCore QtNetwork REQUIRED)
	include(${QT_USE_FILE})
else()
	find_package(Qt5LinguistTools REQUIRED)
	find_package(Qt5Network REQUIRED)
	macro(qt4_add_resources)
		qt5_add_resources(${ARGV})
	endmacro()
	macro(qt4_wrap_ui)
		qt5_wrap_ui(${ARGV})
	endmacro()
	macro(qt4_add_translation)
		qt5_add_translation(${ARGV})
	endmacro()
	macro(qt4_create_translation)
		qt5_create_translation(${ARGV})
	endmacro()
	set(QT_LIBRARIES Qt5::Core Qt5::Network)
	set(QT_INCLUDES ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})
	add_definitions(${Qt5Core_DEFINITIONS} ${Qt5Network_DEFINITIONS})
endif()

if (NOT ZLIB_INCLUDE_DIR OR NOT ZLIB_LIBRARIES)
	find_package(ZLIB REQUIRED)
endif()
find_package(BZip2 REQUIRED)
find_package(Threads REQUIRED)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${ZLIB_INCLUDE_DIR}
	${BZIP2_INCLUDE_DIR}
)

set(WADSEEKER_QT_FILES
	protocols/freedoom/freedoominfoparser.h
	protocols/freedoom/freedoomquery.h
	protocols/idgames/idgamesreply.h
	protocols/wadarchive/wadarchiveclient.h
	protocols/fixednetworkaccessmanager.h
	protocols/networkreplysignalwrapper.h
	protocols/networkreplytimeouter.h
	wadretriever/wadretriever.h
	wwwseeker/idgames.h
	wwwseeker/wwwseeker.h
	zip/unarchive.h
	zip/un7zip.h
	zip/untar.h
	zip/unzip.h
	freedoom.h
	modinstall.h
	wadseeker.h
)

set(WADSEEKER_FILES
	entities/link.cpp
	entities/modfile.cpp
	entities/modset.cpp
	entities/waddownloadinfo.cpp
	protocols/freedoom/freedoominfoparser.cpp
	protocols/freedoom/freedoomquery.cpp
	protocols/idgames/idgamesclient.cpp
	protocols/idgames/idgamesfile.cpp
	protocols/idgames/idgamesfilematcher.cpp
	protocols/idgames/idgamesreply.cpp
	protocols/idgames/idgamesresult.cpp
	protocols/wadarchive/wadarchiveclient.cpp
	protocols/fixednetworkaccessmanager.cpp
	protocols/json.cpp
	protocols/http.cpp
	protocols/networkreply.cpp
	protocols/networkreplysignalwrapper.cpp
	protocols/networkreplytimeouter.cpp
	protocols/urlprovider.cpp
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
	zip/zipfile.cpp
	freedoom.cpp
	ioutils.cpp
	modinstall.cpp
	wadseeker.cpp
	wadseekerversioninfo.cpp
)

# For the purpose of generation proper project files.
set(HEADER_FILES
	${WADSEEKER_QT_FILES}
	entities/link.h
	entities/modfile.h
	entities/modset.h
	entities/waddownloadinfo.h
	protocols/idgames/idgamesclient.h
	protocols/idgames/idgamesfile.h
	protocols/idgames/idgamesfilematcher.h
	protocols/idgames/idgamesresult.h
	protocols/json.h
	protocols/http.h
	protocols/networkreply.h
	protocols/urlprovider.h
	wadretriever/wadinstaller.h
	wwwseeker/entities/fileseekinfo.h
	wwwseeker/htmlparser.h
	wwwseeker/urlparser.h
	zip/zipfile.h
	ioutils.h
	wadseekerexportinfo.h
	wadseekermessagetype.h
	wadseekerversioninfo.h
)

if(NOT Qt5Core_FOUND)
	set(WADSEEKER_FILES ${WADSEEKER_FILES} protocols/fixedftpreply.cpp)
	set(WADSEEKER_QT_FILES ${WADSEEKER_QT_FILES} protocols/fixedftpreply.h)
endif()

include_directories(${QT_INCLUDES})

qt_wrap_cpp(wadseekerQt WADSEEKER_FILES ${WADSEEKER_QT_FILES})

add_library(wadseeker SHARED ${WADSEEKER_FILES} ${HEADER_FILES} $<TARGET_OBJECTS:lzma>)

target_link_libraries(wadseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

if(DEFINED DOOMSEEKER_BINARY_DIR)
	set_target_properties(wadseeker PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${DOOMSEEKER_BINARY_DIR} RUNTIME_OUTPUT_DIRECTORY ${DOOMSEEKER_BINARY_DIR})
endif()

if(NOT WIN32 AND NOT XCODE_VERSION)
	install(TARGETS wadseeker LIBRARY DESTINATION lib)
elseif(WIN32)
	install(TARGETS wadseeker RUNTIME DESTINATION .)
endif()
