cmake_minimum_required(VERSION 2.8.6)
project(DOOMSEEKER)

IF(APPLE)
	# You can set which arch you want by enabling one of these boolean option.
	# EXAMPLE: "cmake -DMAC_ARCH_UNIVERSAL:BOOL=ON"
	# Default is x86
	OPTION(MAC_ARCH_UNIVERSAL
		"Produce a universal binary for Mac."
		OFF)
	OPTION(MAC_ARCH_PPC
		"Produce a PPC binary for Mac."
		OFF)
	OPTION(MAC_ARCH_I386
		"Produce an x86 binary for Mac."
		ON)

	IF(MAC_ARCH_UNIVERSAL)
		set(CMAKE_OSX_ARCHITECTURES ppc;i386)
	ELSEIF(MAC_ARCH_PPC)
		set(CMAKE_OSX_ARCHITECTURES ppc)
	ELSEIF(MAC_ARCH_I386)
		set(CMAKE_OSX_ARCHITECTURES i386)
	ENDIF()

	# The SDK option allows you to support older versions of OS X.
	# The default configuration will build against the default system SDK which is likely 
	# the SDK for your current OS version (10.5 (Leopard), 10.6 (Snow Leopard), etc.)
	# To enable this option: "cmake -DMAC_SDK_10.4:BOOL=ON"
	OPTION(MAC_SDK_10.4
		"Build against the 10.4 \"Tiger\" SDK."
		OFF)

	IF(MAC_SDK_10.4)
		set(CMAKE_OSX_DEPLOYMENT_TARGET 10.4)
		set(CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.4u.sdk)
		set(CMAKE_CXX_COMPILER g++-4.0)
	ENDIF(MAC_SDK_10.4)
ENDIF(APPLE)

find_package( ZLIB )
find_package( BZip2 )

option(FORCE_INTERNAL_ZLIB "Use internal zlib")
option(FORCE_INTERNAL_BZIP2 "Use internal bzip2")

if( ZLIB_FOUND AND NOT FORCE_INTERNAL_ZLIB )
	message( STATUS "Using system zlib" )
else( ZLIB_FOUND AND NOT FORCE_INTERNAL_ZLIB )
	message( STATUS "Using internal zlib" )
	add_subdirectory( zlib )
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

if(MSVC AND NOT MSVC90)
	message(WARNING 
		"Full automatic packaging is only supported for Microsoft Visual C++ 2008. "
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
