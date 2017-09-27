cmake_minimum_required(VERSION 2.8.12)
project(Wadseeker)

if(POLICY CMP0042)
	cmake_policy(SET CMP0042 NEW)
endif()

if (MSVC)
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
endif ()

add_subdirectory(lzma)

include(${CMAKE_SOURCE_DIR}/cmake/Modules/Macros.cmake)

if(NOT FORCE_QT4)
	find_package(Qt5Core)
endif()

if(NOT Qt5Core_FOUND)
	find_package(Qt4 COMPONENTS QtCore QtNetwork REQUIRED)
	set(QT_LIBRARIES Qt4::QtCore Qt4::QtNetwork)
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
endif()

if(NOT TARGET ZLIB::ZLIB)
	find_package_ZLIB(REQUIRED)
endif()
if(NOT TARGET BZip2::BZip2)
	find_package_BZip2(REQUIRED)
endif()
find_package(Threads REQUIRED)

include("wadseekerversiondefs.cmake")
configure_file(wadseekerversiondefs.h.in wadseekerversiondefs.h)

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
	filefind.cpp
	freedoom.cpp
	ioutils.cpp
	modinstall.cpp
	wadseeker.cpp
	wadseekerversioninfo.cpp
)

# For the purpose of generation proper project files.
set(HEADER_FILES
	entities/link.h
	entities/modfile.h
	entities/modset.h
	entities/waddownloadinfo.h
	protocols/freedoom/freedoominfoparser.h
	protocols/freedoom/freedoomquery.h
	protocols/idgames/idgamesclient.h
	protocols/idgames/idgamesfile.h
	protocols/idgames/idgamesfilematcher.h
	protocols/idgames/idgamesreply.h
	protocols/idgames/idgamesresult.h
	protocols/wadarchive/wadarchiveclient.h
	protocols/fixednetworkaccessmanager.h
	protocols/json.h
	protocols/http.h
	protocols/networkreply.h
	protocols/networkreplysignalwrapper.h
	protocols/networkreplytimeouter.h
	protocols/urlprovider.h
	wadretriever/wadinstaller.h
	wadretriever/wadretriever.h
	wwwseeker/entities/fileseekinfo.h
	wwwseeker/htmlparser.h
	wwwseeker/idgames.h
	wwwseeker/urlparser.h
	wwwseeker/wwwseeker.h
	zip/unarchive.h
	zip/un7zip.h
	zip/untar.h
	zip/unzip.h
	zip/zipfile.h
	filefind.h
	freedoom.h
	ioutils.h
	modinstall.h
	wadseeker.h
	wadseekerexportinfo.h
	wadseekermessagetype.h
	wadseekerversiondefs.h
	wadseekerversioninfo.h
)

if(NOT Qt5Core_FOUND)
	set(WADSEEKER_FILES ${WADSEEKER_FILES} protocols/fixedftpreply.cpp)
	set(WADSEEKER_QT_FILES ${WADSEEKER_QT_FILES} protocols/fixedftpreply.h)
endif()

add_library(wadseeker SHARED ${WADSEEKER_FILES} ${HEADER_FILES})

target_include_directories(wadseeker
PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(wadseeker
LINK_PUBLIC
	${QT_LIBRARIES}
LINK_PRIVATE
	${CMAKE_THREAD_LIBS_INIT}
	BZip2::BZip2
	lzma
	ZLIB::ZLIB)

set_target_properties(wadseeker PROPERTIES AUTOMOC ON)

if(DEFINED Doomseeker_BINARY_DIR)
	set_target_properties(wadseeker PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${Doomseeker_BINARY_DIR} RUNTIME_OUTPUT_DIRECTORY ${Doomseeker_BINARY_DIR})
endif()

if(NOT WIN32 AND NOT XCODE_VERSION)
	install(TARGETS wadseeker LIBRARY DESTINATION lib COMPONENT wadseeker)
elseif(WIN32)
	install(TARGETS wadseeker RUNTIME DESTINATION . COMPONENT wadseeker)
endif()

install(SCRIPT InstallHeaders.cmake COMPONENT wadseeker-dev)

# We need a newer CPack version to get desirable results.
if(NOT CMAKE_VERSION VERSION_LESS "3.6.0")
	cpack_add_component(wadseeker
		DISPLAY_NAME libwadseeker
		REQUIRED)

	set(CPACK_PACKAGE_VERSION ${VERSION_STRING})

	set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
	set(CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME "libwadseeker")
	set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_NAME "${CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME}-dev")
	set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_ARCHITECTURE "all")
	if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
		set(CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME ${CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME} PARENT_SCOPE)
		set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_NAME ${CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_NAME} PARENT_SCOPE)
		set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_ARCHITECTURE ${CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_ARCHITECTURE} PARENT_SCOPE)

		file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CPackComponent.version" "${CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME} ${VERSION_STRING}\n${CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_NAME} ${VERSION_STRING}\n")
	endif()
endif()
