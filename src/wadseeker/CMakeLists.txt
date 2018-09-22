#------------------------------------------------------------------------------
# CMakeLists.txt
#------------------------------------------------------------------------------
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301  USA
#------------------------------------------------------------------------------
# Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)
project(Wadseeker)

if(POLICY CMP0042)
	cmake_policy(SET CMP0042 NEW)
endif()

if (MSVC)
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
endif ()

# Multiarch support - CMAKE_INSTALL_LIBDIR will be set by Doomseeker's CMake
# or we will get it from GNUInstallDirs.
if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
	if(UNIX AND NOT APPLE)
		include(GNUInstallDirs)
	else()
		set(CMAKE_INSTALL_LIBDIR "lib")
	endif()
endif()

add_subdirectory(lzma)

include(${CMAKE_SOURCE_DIR}/cmake/Modules/Macros.cmake)

if(NOT FORCE_QT4)
	find_package(Qt5 COMPONENTS Core)
endif()

if(NOT Qt5_FOUND)
	find_package(Qt4 COMPONENTS QtCore QtNetwork REQUIRED)
	set(QT_LIBRARIES Qt4::QtCore Qt4::QtNetwork)
else()
	find_package(Qt5 COMPONENTS LinguistTools Network REQUIRED)
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

set_target_properties(wadseeker PROPERTIES
	AUTOMOC ON
	VERSION ${WADSEEKER_LIBVERSION}
	SOVERSION ${WADSEEKER_SOVERSION}
)

if(DEFINED Doomseeker_BINARY_DIR)
	set_target_properties(wadseeker PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${Doomseeker_BINARY_DIR} RUNTIME_OUTPUT_DIRECTORY ${Doomseeker_BINARY_DIR})
endif()

if(NOT WIN32 AND NOT XCODE_VERSION)
	install(TARGETS wadseeker LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT wadseeker NAMELINK_SKIP)
	install(TARGETS wadseeker LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT wadseeker-dev NAMELINK_ONLY)
	install(FILES ${CMAKE_SOURCE_DIR}/LICENSE.json DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/wadseeker COMPONENT wadseeker)
elseif(WIN32)
	install(TARGETS wadseeker RUNTIME DESTINATION . COMPONENT wadseeker)

	# License files.
	set(LICENSE_FILES
		LICENSE
		LICENSE.json
	)
	foreach (LICFILE ${LICENSE_FILES})
		install(FILES "${CMAKE_SOURCE_DIR}/${LICFILE}" DESTINATION .
			COMPONENT wadseeker
			RENAME "${LICFILE}.txt")
	endforeach(LICFILE)
endif()

install(SCRIPT InstallHeaders.cmake COMPONENT wadseeker-dev)

# We need a newer CPack version to get desirable results.
if(NOT CMAKE_VERSION VERSION_LESS "3.6.0")
	# CPack can autogenerate this file, but since we have a postprocessing step
	# to correct the versions per component we're better off generating it
	# manually. This means there will be a lintian error until that step is run.
	configure_file(deb-extra/shlibs.in ${CMAKE_CURRENT_BINARY_DIR}/shlibs)

	cpack_add_component(wadseeker
		DISPLAY_NAME libwadseeker
		DESCRIPTION "Library for automatically locating and downloading mods for Doom."
		REQUIRED)
	parent_scope_component(wadseeker)

	set(CPACK_PACKAGE_VERSION ${VERSION_STRING})

	set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
	set(CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME "libwadseeker1")
	set(CPACK_DEBIAN_WADSEEKER_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/deb-extra/triggers;${CMAKE_CURRENT_BINARY_DIR}/shlibs")
	set(CPACK_DEBIAN_WADSEEKER_PACKAGE_SECTION "libs")
	set(CPACK_COMPONENT_WADSEEKER_DESCRIPTION "Library for locating and downloading Doom mods.\n Searching is done against a given list of common servers including the id\n games archive. Wadseeker is based around Qt core and does not provide a GUI\n itself.")
	set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_NAME "libwadseeker-dev")
	set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME}, qtbase5-dev")
	set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_SECTION "libdevel")
	set(CPACK_COMPONENT_WADSEEKER-DEV_DESCRIPTION "Header files for libwadseeker.\n Wadseeker is a library for locating and downloading Doom mods.")
	if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
		set(CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME ${CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME} PARENT_SCOPE)
		set(CPACK_DEBIAN_WADSEEKER_PACKAGE_CONTROL_EXTRA ${CPACK_DEBIAN_WADSEEKER_PACKAGE_CONTROL_EXTRA} PARENT_SCOPE)
		set(CPACK_DEBIAN_WADSEEKER_PACKAGE_SECTION ${CPACK_DEBIAN_WADSEEKER_PACKAGE_SECTION} PARENT_SCOPE)
		set(CPACK_COMPONENT_WADSEEKER_DESCRIPTION ${CPACK_COMPONENT_WADSEEKER_DESCRIPTION} PARENT_SCOPE)
		set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_NAME ${CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_NAME} PARENT_SCOPE)
		set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_DEPENDS ${CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_DEPENDS} PARENT_SCOPE)
		set(CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_SECTION ${CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_SECTION} PARENT_SCOPE)
		set(CPACK_COMPONENT_WADSEEKER-DEV_DESCRIPTION ${CPACK_COMPONENT_WADSEEKER-DEV_DESCRIPTION} PARENT_SCOPE)

		file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CPackComponent.version" "${CPACK_DEBIAN_WADSEEKER_PACKAGE_NAME} ${VERSION_STRING}\n${CPACK_DEBIAN_WADSEEKER-DEV_PACKAGE_NAME} ${VERSION_STRING}\n")
	endif()
endif()
