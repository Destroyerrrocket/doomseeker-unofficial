cmake_minimum_required(VERSION 2.8.7)
project(DOOMSEEKER)

if(NOT WIN32)
	find_package( ZLIB )
	find_package( BZip2 )
endif()

option(FORCE_QT4 "Force Qt4" NO)
if(WIN32)
	set(FORCE_INTERNAL_ZLIB YES)
	set(FORCE_INTERNAL_BZIP2 YES)
else()
	option(FORCE_INTERNAL_ZLIB "Use internal zlib")
	option(FORCE_INTERNAL_BZIP2 "Use internal bzip2")
endif()

if( ZLIB_FOUND AND NOT FORCE_INTERNAL_ZLIB )
	message( STATUS "Using system zlib" )
else( ZLIB_FOUND AND NOT FORCE_INTERNAL_ZLIB )
	message( STATUS "Using internal zlib" )
	set(SKIP_INSTALL_ALL ON)
	add_subdirectory( zlib )
	unset(SKIP_INSTALL_ALL)
	set( ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zlib )
	set( ZLIB_LIBRARIES z )
	set( ZLIB_LIBRARY z )
endif( ZLIB_FOUND AND NOT FORCE_INTERNAL_ZLIB )

if( BZIP2_FOUND AND NOT FORCE_INTERNAL_BZIP2 )
	message( STATUS "Using system bzip2 library" )
else( BZIP2_FOUND AND NOT FORCE_INTERNAL_BZIP2 )
	message( STATUS "Using internal bzip2 library" )
	add_subdirectory( bzip2 )
	set( BZIP2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bzip2" )
	set( BZIP2_LIBRARIES bz2 )
	set( BZIP2_LIBRARY bz2 )
endif( BZIP2_FOUND AND NOT FORCE_INTERNAL_BZIP2 )

if(MSVC AND NOT MSVC90 AND NOT MSVC12)
	message(WARNING
		"Full automatic packaging is only supported for Microsoft Visual C++ 2008 and 2013. "
		"Remember that redist needs to be accessible both by the main program "
		"and by the engines/ directory. Copy over Manifest and redist DLLs to "
		"engines/ directory if you do not plan on installing redist on target "
		"machines."
	)
endif()

if(WIN32 OR APPLE)
	set(WITH_AUTOUPDATES 1)
	add_subdirectory(tools/updateinstaller)
endif(WIN32 OR APPLE)

add_subdirectory(tools/updaterevision)

add_subdirectory(src/core)
add_subdirectory(src/plugins)
add_subdirectory(src/wadseeker)

message(STATUS "'INSTALL' will dump files to \"${CMAKE_INSTALL_PREFIX}\"")
