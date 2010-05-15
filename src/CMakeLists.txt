cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
	cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)

find_package(Qt4 REQUIRED)
find_package(ZLIB REQUIRED)
include(${QT_USE_FILE})

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)
include_directories(${QT_INCLUDES})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${ZLIB_INCLUDE_DIR})

if(WIN32)
	add_definitions(-DMODE_MAIN)
endif(WIN32)
add_definitions("-DINSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

qt4_wrap_ui(doomseekerUI
	gui/aboutDlg.ui
	gui/addBuddyDlg.ui
	gui/cfgAppearance.ui
	gui/cfgcustomservers.ui
	gui/cfgFilePaths.ui
	gui/cfgQuery.ui
	gui/configureDlg.ui
	gui/copytextdlg.ui
	gui/createserver.ui
	gui/dockBuddiesList.ui
	gui/logdock.ui
	gui/mainwindow.ui
	gui/passwordDlg.ui
	gui/remoteconsole.ui
	gui/wadseekerconfiggeneral.ui
	gui/wadseekerconfigidgames.ui
	gui/wadseekerconfigsites.ui
	gui/wadseekerinterface.ui
	gui/widgets/serverconsole.ui
)

set(DOOMSEEKER_QT_FILES
	gui/aboutDlg.h
	gui/cfgAppearance.h
	gui/cfgcustomservers.h	
	gui/cfgFilePaths.h
	gui/cfgQuery.h
	gui/configBase.h
	gui/configureDlg.h
	gui/copytextdlg.h
	gui/createserver.h
	gui/dockBuddiesList.h
	gui/logdock.h
	gui/mainwindow.h
	gui/passwordDlg.h
	gui/remoteconsole.h
	gui/serverlist.h
	gui/standardserverconsole.h
	gui/wadseekerconfiggeneral.h
	gui/wadseekerconfigidgames.h
	gui/wadseekerconfigsites.h
	gui/wadseekerinterface.h
	gui/models/serverlistmodel.h
	gui/widgets/dockedwidgets.h
	gui/widgets/serverconsole.h
	gui/widgets/serverlistview.h
	gui/widgets/serversstatuswidget.h
	sdeapi/config.hpp
	serverapi/rconprotocol.h
	serverapi/server.h
	customservers.h
	ip2c.h
	log.h
	masterclient.h
	refresher.h
)

qt_wrap_cpp(doomseekerWraps QT_FILES ${DOOMSEEKER_QT_FILES})

qt4_add_resources(doomseekerResources
	../media/resources.qrc
)

set(SOURCE_FILES
	gui/aboutDlg.cpp
	gui/cfgAppearance.cpp
	gui/cfgcustomservers.cpp	
	gui/cfgFilePaths.cpp
	gui/cfgQuery.cpp
	gui/commonGUI.cpp
	gui/configureDlg.cpp
	gui/copytextdlg.cpp
	gui/createserver.cpp
	gui/dockBuddiesList.cpp
	gui/logdock.cpp
	gui/mainwindow.cpp
	gui/passwordDlg.cpp
	gui/remoteconsole.cpp
	gui/serverlist.cpp
	gui/standardserverconsole.cpp
	gui/wadseekerconfiggeneral.cpp
	gui/wadseekerconfigidgames.cpp
	gui/wadseekerconfigsites.cpp
	gui/wadseekerinterface.cpp
	gui/helpers/playersdiagram.cpp
	gui/models/serverlistcolumn.cpp
	gui/models/serverlistmodel.cpp
	gui/models/serverlistrowhandler.cpp
	gui/widgets/serverconsole.cpp
	gui/widgets/serverlistcontextmenu.cpp
	gui/widgets/serverlistview.cpp
	gui/widgets/serversstatuswidget.cpp
	sdeapi/config.cpp
	sdeapi/pluginloader.cpp
	sdeapi/scanner.cpp
	serverapi/tooltips/gameinfotip.cpp
	serverapi/tooltips/generalinfotip.cpp
	serverapi/tooltips/playertable.cpp
	serverapi/binaries.cpp
	serverapi/gamerunner.cpp
	serverapi/player.cpp
	serverapi/playerslist.cpp
	serverapi/rconprotocol.cpp
	serverapi/server.cpp
	serverapi/tooltipgenerator.cpp
	apprunner.cpp
	customservers.cpp
	ip2c.cpp
	log.cpp
	main.cpp
	masterclient.cpp
	pathfinder.cpp
	refresher.cpp
	strings.cpp	
)

# For the purpose of generation of proper project files.
set(HEADER_FILES
	${DOOMSEEKER_QT_FILES}
	gui/models/serverlistcolumn.h
	gui/models/serverlistrowhandler.h
	gui/helpers/playersdiagram.h
	sdeapi/pluginloader.hpp
	sdeapi/scanner.hpp
	serverapi/tooltips/gameinfotip.h
	serverapi/tooltips/generalinfotip.h
	serverapi/tooltips/playertable.h
	serverapi/binaries.h
	serverapi/gamerunner.h
	serverapi/player.h
	serverapi/playerslist.h
	serverapi/tooltipgenerator.h
	apprunner.h
	global.h
	main.h
	messageresult.h
	pathfinder.h
	strings.h
	version.h
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
	${HEADER_FILES}
)

target_link_libraries(doomseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} wadseeker)

# Allow plugins to link with doomseeker
set_target_properties(doomseeker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR} ENABLE_EXPORTS 1)
if(WIN32)
	set_target_properties(doomseeker PROPERTIES LINK_FLAGS_RELEASE "-Wl,-subsystem,windows")
	set_target_properties(doomseeker PROPERTIES LINK_FLAGS_MINSIZEREL "-Wl,-subsystem,windows")
endif(WIN32)

if(NOT WIN32)
install(TARGETS doomseeker RUNTIME DESTINATION bin)
endif(NOT WIN32)

if(UNIX)
	install(FILES ${CMAKE_BINARY_DIR}/media/icon.png DESTINATION share/doomseeker)
	install(FILES ${CMAKE_BINARY_DIR}/media/Doomseeker.desktop DESTINATION /usr/share/applications)
endif(UNIX)
