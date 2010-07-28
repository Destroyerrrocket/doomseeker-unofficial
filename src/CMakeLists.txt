cmake_minimum_required(VERSION 2.4)

# Regarding Microsoft Visual Studio:
# Creating proper builds with MSVC required setting subsystem to windows,
# otherwise an unwanted console window was created. However when subsystem is
# set to windows MSVC uses WinMain() as the entry point, while MinGW simply 
# keeps using the main() function. Also note that since console window is useful
# for debugging, Debug builds still have subsystem set to console.
#
# To solve all the issues WinMain() has been implemented and is now used 
# depending on certain defines: if _MSC_VER is defined it means that user is
# attempting to create a build with MSVC. Also if _DEBUG is NOT defined it means
# that user chose one of the Release builds. When both of these conditions are 
# met then, and only then, WinMain() is used instead of main() as the
# entry point.
#
# Conclusion: If current MSVC subsystem setup is changed the code may not 
# compile, as the proper entry point will be missing.

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
	cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)

# Get the SVN revision
get_target_property( UPDATEREVISION_EXE updaterevision LOCATION )

add_custom_target(revision_check ALL
	COMMAND ${UPDATEREVISION_EXE} . ${CMAKE_CURRENT_SOURCE_DIR}/svnrevision.h
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS updaterevision
)

find_package(Qt4 REQUIRED)
find_package(ZLIB REQUIRED)
include(${QT_USE_FILE})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../
	${CMAKE_CURRENT_SOURCE_DIR} 
	${CMAKE_CURRENT_SOURCE_DIR}/gui/
	${QT_INCLUDES} 
	${CMAKE_CURRENT_BINARY_DIR} 
	${ZLIB_INCLUDE_DIR}
)

if(WIN32)
	add_definitions(-DMODE_MAIN)
endif(WIN32)
add_definitions("-DINSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

qt4_wrap_ui(doomseekerUI
	gui/irc/ircdock.ui
	gui/irc/ircdocktabcontents.ui
	gui/irc/ircnetworkselectionbox.ui
	gui/widgets/serverconsole.ui
	gui/aboutDlg.ui
	gui/addBuddyDlg.ui
	gui/cfgAppearance.ui
	gui/cfgcustomservers.ui
	gui/cfgFilePaths.ui
	gui/cfgip2c.ui
	gui/cfgQuery.ui
	gui/configureDlg.ui
	gui/copytextdlg.ui
	gui/createserver.ui
	gui/engineConfigBase.ui
	gui/dockBuddiesList.ui
	gui/ip2cupdatebox.ui	
	gui/logdock.ui
	gui/mainwindow.ui
	gui/passwordDlg.ui
	gui/remoteconsole.ui
	gui/wadseekerconfigappearance.ui
	gui/wadseekerconfiggeneral.ui
	gui/wadseekerconfigidgames.ui
	gui/wadseekerconfigsites.ui
	gui/wadseekerinterface.ui
)

set(DOOMSEEKER_QT_FILES
	gui/irc/ircdock.h
	gui/irc/ircdocktabcontents.h
	gui/irc/ircnetworkselectionbox.h
	gui/models/serverlistmodel.h
	gui/widgets/colorbutton.h
	gui/widgets/dockedwidgets.h
	gui/widgets/serverconsole.h
	gui/widgets/serverlistview.h
	gui/widgets/serversstatuswidget.h	
	gui/aboutDlg.h
	gui/cfgAppearance.h
	gui/cfgcustomservers.h	
	gui/cfgFilePaths.h
	gui/cfgip2c.h
	gui/cfgQuery.h
	gui/configBase.h
	gui/configureDlg.h
	gui/copytextdlg.h
	gui/createserver.h
	gui/dockBuddiesList.h
	gui/engineConfigBase.h
	gui/ip2cupdatebox.h
	gui/logdock.h
	gui/mainwindow.h
	gui/passwordDlg.h
	gui/remoteconsole.h
	gui/serverlist.h
	gui/standardserverconsole.h
	gui/wadseekerconfigappearance.h
	gui/wadseekerconfiggeneral.h
	gui/wadseekerconfigidgames.h
	gui/wadseekerconfigsites.h
	gui/wadseekerinterface.h
	irc/ircadapterbase.h
	irc/ircclient.h
	irc/ircnetworkadapter.h
	sdeapi/config.hpp
	serverapi/rconprotocol.h
	serverapi/server.h
	customservers.h
	ip2c.h
	ip2cparser.h
	ip2cupdater.h
	log.h
	masterclient.h
	mastermanager.h
	refresher.h
	socketsignalsadapter.h
)

qt_wrap_cpp(doomseekerWraps QT_FILES ${DOOMSEEKER_QT_FILES})

qt4_add_resources(doomseekerResources
	${DOOMSEEKER_SOURCE_DIR}/media/resources.qrc
)

set(SOURCE_FILES
	gui/helpers/playersdiagram.cpp
	gui/irc/ircdock.cpp	
	gui/irc/ircdocktabcontents.cpp
	gui/irc/ircnetworkselectionbox.cpp
	gui/models/serverlistcolumn.cpp
	gui/models/serverlistmodel.cpp
	gui/models/serverlistrowhandler.cpp
	gui/widgets/colorbutton.cpp
	gui/widgets/serverconsole.cpp
	gui/widgets/serverlistcontextmenu.cpp
	gui/widgets/serverlistview.cpp
	gui/widgets/serversstatuswidget.cpp
	gui/aboutDlg.cpp
	gui/cfgAppearance.cpp
	gui/cfgcustomservers.cpp	
	gui/cfgFilePaths.cpp
	gui/cfgip2c.cpp
	gui/cfgQuery.cpp
	gui/commonGUI.cpp
	gui/configureDlg.cpp
	gui/copytextdlg.cpp
	gui/createserver.cpp
	gui/dockBuddiesList.cpp
	gui/engineConfigBase.cpp
	gui/ip2cupdatebox.cpp
	gui/logdock.cpp
	gui/mainwindow.cpp
	gui/passwordDlg.cpp
	gui/remoteconsole.cpp
	gui/serverlist.cpp
	gui/standardserverconsole.cpp
	gui/wadseekerconfigappearance.cpp	
	gui/wadseekerconfiggeneral.cpp
	gui/wadseekerconfigidgames.cpp
	gui/wadseekerconfigsites.cpp
	gui/wadseekerinterface.cpp
	irc/ircclient.cpp
	irc/ircnetworkadapter.cpp
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
	tests/testbase.cpp
	tests/testcore.cpp
	tests/testdatapaths.cpp
	tests/testini.cpp
	tests/testruns.cpp
	apprunner.cpp
	customservers.cpp
	datapaths.cpp
	doomseekerfilepaths.cpp
	ini.cpp
	ip2c.cpp
	ip2cparser.cpp
	ip2cupdater.cpp
	log.cpp
	main.cpp
	masterclient.cpp
	mastermanager.cpp
	pathfinder.cpp
	refresher.cpp
	strings.cpp	
)

# For the purpose of generation of proper project files.
set(HEADER_FILES
	${DOOMSEEKER_QT_FILES}
	gui/helpers/playersdiagram.h
	gui/models/serverlistcolumn.h
	gui/models/serverlistrowhandler.h
	irc/ircnetworkconnectioninfo.h
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
	tests/testbase.h	
	tests/testcore.h
	tests/testdatapaths.h	
	tests/testini.h
	tests/testruns.h
	apprunner.h
	datapaths.h	
	doomseekerfilepaths.h
	global.h
	ini.h
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
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/windows.o
			COMMAND windres -o ${CMAKE_CURRENT_BINARY_DIR}/windows.o -i ${DOOMSEEKER_SOURCE_DIR}/media/windows.rc
      WORKING_DIRECTORY ${DOOMSEEKER_SOURCE_DIR}/media
			COMMENT "Generating windows resources (windows.rc)"
		)

		set(SOURCE_FILES ${SOURCE_FILES} windows.o)
	else(CMAKE_COMPILER_IS_GNUCXX)
		set(SOURCE_FILES ${SOURCE_FILES} ${DOOMSEEKER_SOURCE_DIR}/media/windows.rc)
	endif(CMAKE_COMPILER_IS_GNUCXX)
endif(WIN32)

add_executable(doomseeker
	${doomseekerUI}
	${doomseekerResources}
	${QT_FILES}
	${SOURCE_FILES}
	${HEADER_FILES}
)

# Make sure the svnrevision.h file is created before we attempt to compile
# Doomseeker.
add_dependencies(doomseeker revision_check)

target_link_libraries(doomseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} wadseeker)

# Allow plugins to link with doomseeker
set_target_properties(doomseeker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR} ENABLE_EXPORTS 1)
if(WIN32)
	if(MSVC)
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
	else(MSVC)
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_RELEASE "-Wl,-subsystem,windows")
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_MINSIZEREL "-Wl,-subsystem,windows")
	endif(MSVC)
	
endif(WIN32)

if(NOT WIN32 AND NOT XCODE_VERSION)
	install(TARGETS doomseeker RUNTIME DESTINATION bin)
endif(NOT WIN32 AND NOT XCODE_VERSION)

if(UNIX AND NOT APPLE)
	install(FILES ${CMAKE_BINARY_DIR}/media/icon.png DESTINATION share/doomseeker)
	install(FILES ${CMAKE_BINARY_DIR}/media/Doomseeker.desktop DESTINATION /usr/share/applications)
endif(UNIX AND NOT APPLE)
