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
# attempting to create a build with MSVC. Also if NDEBUG is defined it means
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

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)

add_definitions("-DINSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

qt4_wrap_ui(doomseekerUI
	gui/configuration/irc/cfgircappearance.ui
	gui/configuration/irc/cfgircdefinenetworkdialog.ui
	gui/configuration/irc/cfgircnetworks.ui
	gui/configuration/irc/cfgircsounds.ui
	gui/configuration/cfgappearance.ui
	gui/configuration/cfgcustomservers.ui
	gui/configuration/cfgfilepaths.ui
	gui/configuration/cfgip2country.ui
	gui/configuration/cfgquery.ui
	gui/configuration/configurationdialog.ui
	gui/configuration/cfgwadseekerappearance.ui
	gui/configuration/cfgwadseekergeneral.ui
	gui/configuration/cfgwadseekeridgames.ui
	gui/configuration/cfgwadseekersites.ui
	gui/configuration/engineconfigurationbasebox.ui
	gui/irc/ircdock.ui
	gui/irc/ircdocktabcontents.ui
	gui/irc/ircnetworkselectionbox.ui
	gui/widgets/serverconsole.ui
	gui/aboutDlg.ui
	gui/addBuddyDlg.ui
	gui/copytextdlg.ui
	gui/createserver.ui
	gui/demomanager.ui
	gui/dockBuddiesList.ui
	gui/ip2cupdatebox.ui
	gui/logdock.ui
	gui/mainwindow.ui
	gui/passwordDlg.ui
	gui/remoteconsole.ui
	gui/serverfilterdock.ui
	gui/wadseekerinterface.ui
)

set(DOOMSEEKER_QT_FILES
	gui/configuration/irc/cfgircdefinenetworkdialog.h
	gui/configuration/irc/cfgircnetworks.h
	gui/configuration/irc/cfgircsounds.h
	gui/configuration/cfgappearance.h
	gui/configuration/cfgcustomservers.h
	gui/configuration/cfgfilepaths.h
	gui/configuration/cfgip2country.h
	gui/configuration/cfgquery.h
	gui/configuration/cfgwadseekerappearance.h
	gui/configuration/cfgwadseekergeneral.h
	gui/configuration/cfgwadseekeridgames.h
	gui/configuration/cfgwadseekersites.h
	gui/configuration/configurationbasebox.h
	gui/configuration/configurationdialog.h
	gui/configuration/doomseekerconfigurationdialog.h
	gui/configuration/engineconfigurationbasebox.h
	gui/irc/ircdock.h
	gui/irc/ircdocktabcontents.h
	gui/irc/ircnetworkselectionbox.h
	gui/models/serverlistmodel.h
	gui/widgets/colorbutton.h
	gui/widgets/dockedwidgets.h
	gui/widgets/fontbutton.h
	gui/widgets/importantmessageswidget.h
	gui/widgets/memorylineedit.h
	gui/widgets/serverconsole.h
	gui/widgets/serverlistview.h
	gui/widgets/serversstatuswidget.h
	gui/aboutDlg.h
	gui/copytextdlg.h
	gui/createserver.h
	gui/demomanager.h
	gui/dockBuddiesList.h
	gui/logdock.h
	gui/mainwindow.h
	gui/passwordDlg.h
	gui/remoteconsole.h
	gui/serverfilterdock.h
	gui/serverlist.h
	gui/standardserverconsole.h
	gui/wadseekerinterface.h
	irc/ircadapterbase.h
	irc/ircchanneladapter.h
	irc/ircclient.h
	irc/ircglobalmessages.h
	irc/ircnetworkadapter.h
	irc/ircrequestparser.h
	irc/ircresponseparser.h
	sdeapi/config.hpp
	serverapi/message.h
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
	configuration/doomseekerconfig.cpp
	gui/configuration/irc/cfgircappearance.cpp
	gui/configuration/irc/cfgircdefinenetworkdialog.cpp
	gui/configuration/irc/cfgircnetworks.cpp
	gui/configuration/irc/cfgircsounds.cpp
	gui/configuration/irc/ircconfigurationdialog.cpp
	gui/configuration/cfgappearance.cpp
	gui/configuration/cfgcustomservers.cpp
	gui/configuration/cfgfilepaths.cpp
	gui/configuration/cfgip2country.cpp
	gui/configuration/cfgquery.cpp
	gui/configuration/cfgwadseekerappearance.cpp
	gui/configuration/cfgwadseekergeneral.cpp
	gui/configuration/cfgwadseekeridgames.cpp
	gui/configuration/cfgwadseekersites.cpp
	gui/configuration/configurationdialog.cpp
	gui/configuration/doomseekerconfigurationdialog.cpp
	gui/configuration/engineconfigurationbasebox.cpp
	gui/entity/serverlistfilterinfo.cpp
	gui/helpers/playersdiagram.cpp
	gui/irc/ircdock.cpp
	gui/irc/ircdocktabcontents.cpp
	gui/irc/ircnetworkselectionbox.cpp
	gui/irc/ircsounds.cpp
	gui/models/serverlistcolumn.cpp
	gui/models/serverlistmodel.cpp
	gui/models/serverlistproxymodel.cpp
	gui/models/serverlistrowhandler.cpp
	gui/widgets/colorbutton.cpp
	gui/widgets/fontbutton.cpp
	gui/widgets/importantmessageswidget.cpp
	gui/widgets/memorylineedit.cpp
	gui/widgets/serverconsole.cpp
	gui/widgets/serverlistcontextmenu.cpp
	gui/widgets/serverlistview.cpp
	gui/widgets/serversstatuswidget.cpp
	gui/aboutDlg.cpp
	gui/commonGUI.cpp
	gui/copytextdlg.cpp
	gui/createserver.cpp
	gui/demomanager.cpp
	gui/dockBuddiesList.cpp
	gui/ip2cupdatebox.cpp
	gui/logdock.cpp
	gui/mainwindow.cpp
	gui/passwordDlg.cpp
	gui/remoteconsole.cpp
	gui/serverfilterdock.cpp
	gui/serverlist.cpp
	gui/standardserverconsole.cpp
	gui/wadseekerinterface.cpp
	irc/configuration/ircconfig.cpp
	irc/constants/ircresponsetype.cpp
    irc/entities/ircnetworkentity.cpp
    irc/entities/ircresponseparseresult.cpp    
	irc/ircchanneladapter.cpp
	irc/ircchatadapter.cpp
	irc/ircclient.cpp
	irc/ircdelayedoperation.cpp
	irc/ircdelayedoperationlist.cpp
	irc/ircglobal.cpp
	irc/ircglobalmessages.cpp
	irc/ircmessageclass.cpp
	irc/ircnetworkadapter.cpp
	irc/ircprivadapter.cpp
	irc/ircrequestparser.cpp
	irc/ircresponseparser.cpp
	irc/ircuserinfo.cpp
	irc/ircuserlist.cpp
	sdeapi/config.cpp
	sdeapi/pluginloader.cpp
	sdeapi/scanner.cpp
	serverapi/tooltips/gameinfotip.cpp
	serverapi/tooltips/generalinfotip.cpp
	serverapi/tooltips/playertable.cpp
	serverapi/binaries.cpp
	serverapi/buddyinfo.cpp
	serverapi/gamerunner.cpp
	serverapi/message.cpp
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
	tests/teststrings.cpp
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
	random.cpp
	refresher.cpp
	strings.cpp
	version.cpp
)

# For the purpose of generation of proper project files.
set(HEADER_FILES
	${DOOMSEEKER_QT_FILES}
	configuration/doomseekerconfig.h
	gui/configuration/irc/cfgircappearance.h
	gui/configuration/irc/ircconfigurationdialog.h
	gui/entity/serverlistfilterinfo.h
	gui/helpers/playersdiagram.h
	gui/irc/ircsounds.h
	gui/models/serverlistcolumn.h
	gui/models/serverlistproxymodel.h
	gui/models/serverlistrowhandler.h
	gui/widgets/qtabwidgetext.h
	gui/ip2cupdatebox.h
	irc/configuration/ircconfig.h
	irc/constants/ircresponsetype.h
    irc/entities/ircnetworkentity.h
    irc/entities/ircresponseparseresult.h
	irc/ircchatadapter.h
	irc/ircdelayedoperation.h
	irc/ircdelayedoperationlist.h
	irc/ircglobal.h
	irc/ircmessageclass.h
	irc/ircnetworkconnectioninfo.h
	irc/ircprivadapter.h
	irc/ircuserinfo.h
	irc/ircuserlist.h
	sdeapi/pluginloader.hpp
	sdeapi/scanner.hpp
	serverapi/tooltips/gameinfotip.h
	serverapi/tooltips/generalinfotip.h
	serverapi/tooltips/playertable.h
	serverapi/binaries.h
	serverapi/buddyinfo.h
	serverapi/gamerunner.h
	serverapi/player.h
	serverapi/playerslist.h
	serverapi/serverstructs.h
	serverapi/tooltipgenerator.h
	tests/testbase.h
	tests/testcore.h
	tests/testdatapaths.h
	tests/testini.h
	tests/testruns.h
	tests/teststrings.h
	apprunner.h
	datapaths.h
	doomseekerfilepaths.h
	global.h
	ini.h
	main.h
	random.h
	pathfinder.h
	qtmetapointer.h
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

target_link_libraries(doomseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} wadseeker ${CMAKE_DL_LIBS})

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
