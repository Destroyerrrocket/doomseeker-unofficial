cmake_minimum_required(VERSION 2.4)
project(DOOMSEEKER)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR} CACHE PATH "Directory in which to build the binary.")

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

add_subdirectory(tools/updaterevision)

add_subdirectory(src/core)
add_subdirectory(src/plugins)
add_subdirectory(src/wadseeker)
