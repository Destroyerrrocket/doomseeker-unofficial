cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(Qt4 REQUIRED)
find_package(ZLIB REQUIRED)
include(${QT_USE_FILE})

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)
include_directories(${QT_INCLUDES})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
	add_definitions(-DMODE_MAIN)
endif(WIN32)

qt4_wrap_ui(doomseekerUI
	gui/aboutDlg.ui
	gui/addBuddyDlg.ui
	gui/cfgFilePaths.ui
	gui/cfgQuery.ui
	gui/configureDlg.ui
	gui/copytextdlg.ui
	gui/dockBuddiesList.ui
	gui/dockserverinfo.ui
	gui/mainwindow.ui
	gui/passwordDlg.ui
	gui/wadseekerconfig.ui
	gui/wadseekerinterface.ui
)

qt_wrap_cpp(doomseekerWraps QT_FILES
	gui/aboutDlg.h
	gui/cfgFilePaths.h
	gui/cfgQuery.h
	gui/configBase.h
	gui/configureDlg.h
	gui/copytextdlg.h
	gui/dockBuddiesList.h
	gui/dockserverinfo.h
	gui/mainwindow.h
	gui/passwordDlg.h
	gui/serverlist.h
	gui/wadseekerconfig.h
	gui/wadseekerinterface.h
	gui/models/serverlistmodel.h
	gui/widgets/dockedwidgets.h
	gui/widgets/serverlistview.h
	sdeapi/config.hpp
	ip2c.h
	masterclient.h
	server.h
)

qt4_add_resources(doomseekerResources
	../media/resources.qrc
)

set(SOURCE_FILES
	gui/aboutDlg.cpp
	gui/cfgFilePaths.cpp
	gui/cfgQuery.cpp
	gui/configureDlg.cpp
	gui/copytextdlg.cpp
	gui/dockBuddiesList.cpp
	gui/dockserverinfo.cpp
	gui/mainwindow.cpp
	gui/passwordDlg.cpp
	gui/serverlist.cpp
	gui/wadseekerconfig.cpp
	gui/wadseekerinterface.cpp
	gui/models/serverlistmodel.cpp
	gui/widgets/serverlistview.cpp
	huffman/huffman.cpp
	sdeapi/config.cpp
	sdeapi/pluginloader.cpp
	sdeapi/scanner.cpp
	ip2c.cpp
	main.cpp
	masterclient.cpp
	pathfinder.cpp
	server.cpp
)

if(WIN32)
	# compile the Windows resource file.  How we do this depends on the compiler.
	if(CMAKE_COMPILER_IS_GNUCXX)
		add_custom_command(
			OUTPUT ${OUTPUT_DIR}/src/windows.o
			COMMAND windres -o ${OUTPUT_DIR}/src/windows.o -i ${CMAKE_CURRENT_SOURCE_DIR}/../media/windows.rc
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../media
			COMMENT "Generating windows resources (windows.rc)"
		)

		set(SOURCE_FILES ${SOURCE_FILES} windows.o)
	else(CMAKE_COMPILER_IS_GNUCXX)
		set(SOURCE_FILES ${SOURCE_FILES} ../media/windows.rc)
	endif(CMAKE_COMPILER_IS_GNUCXX)
endif(WIN32)

add_executable(doomseeker
	${doomseekerUI}
	${doomseekerResources}
	${QT_FILES}
	${SOURCE_FILES}
)

target_link_libraries(doomseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} wadseeker)

# Allow plugins to link with doomseeker
set_target_properties(doomseeker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR} ENABLE_EXPORTS 1)
if(WIN32)
	set_target_properties(doomseeker PROPERTIES LINK_FLAGS_RELEASE "-Wl,-subsystem,windows")
	set_target_properties(doomseeker PROPERTIES LINK_FLAGS_MINSIZEREL "-Wl,-subsystem,windows")
endif(WIN32)
