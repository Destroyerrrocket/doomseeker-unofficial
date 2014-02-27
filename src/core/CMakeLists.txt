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

if(WITH_AUTOUPDATES)
	add_definitions(-DWITH_AUTOUPDATES)
endif(WITH_AUTOUPDATES)

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
	gui/configuration/cfgautoupdates.ui
	gui/configuration/cfgcustomservers.ui
	gui/configuration/cfgfilepaths.ui
	gui/configuration/cfgip2country.ui
	gui/configuration/cfgquery.ui
	gui/configuration/cfgserverpasswords.ui
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
	gui/aboutdialog.ui
	gui/addBuddyDlg.ui
	gui/copytextdlg.ui
	gui/createserverdialog.ui
	gui/demomanager.ui
	gui/dockBuddiesList.ui
	gui/ip2cupdatebox.ui
	gui/logdock.ui
	gui/mainwindow.ui
	gui/passwordDlg.ui
	gui/rconpassworddialog.ui
	gui/remoteconsole.ui
	gui/serverfilterdock.ui
	gui/wadseekerinterface.ui
)

set(DOOMSEEKER_QT_FILES
	gui/configuration/irc/cfgircappearance.h
	gui/configuration/irc/cfgircdefinenetworkdialog.h
	gui/configuration/irc/cfgircnetworks.h
	gui/configuration/irc/cfgircsounds.h
	gui/configuration/irc/ircconfigurationdialog.h
	gui/configuration/cfgappearance.h
	gui/configuration/cfgautoupdates.h
	gui/configuration/cfgcustomservers.h
	gui/configuration/cfgfilepaths.h
	gui/configuration/cfgip2country.h
	gui/configuration/cfgquery.h
	gui/configuration/cfgserverpasswords.h
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
	gui/models/serverlistcolumn.h
	gui/models/serverlistmodel.h
	gui/models/serverlistrowhandler.h
	gui/widgets/colorbutton.h
	gui/widgets/createserverdialogpage.h
	gui/widgets/dndfilesystemlistview.h
	gui/widgets/dockedwidgets.h
	gui/widgets/fontbutton.h
	gui/widgets/importantmessageswidget.h
	gui/widgets/memorylineedit.h
	gui/widgets/multicombobox.h
	gui/widgets/serverconsole.h
	gui/widgets/serverfilterbuildermenu.h
	gui/widgets/serverlistcontextmenu.h
	gui/widgets/serverlistview.h
	gui/widgets/serversstatuswidget.h
	gui/widgets/tablewidgetmouseaware.h
	gui/widgets/wadseekersitestable.h
	gui/widgets/wadseekerwadstable.h
	gui/aboutdialog.h
	gui/copytextdlg.h
	gui/createserverdialog.h
	gui/demomanager.h
	gui/dockBuddiesList.h
	gui/ip2cupdatebox.h
	gui/logdock.h
	gui/mainwindow.h
	gui/passwordDlg.h
	gui/rconpassworddialog.h
	gui/remoteconsole.h
	gui/serverfilterdock.h
	gui/serverlist.h
	gui/standardserverconsole.h
	gui/wadseekerinterface.h
	ini/ini.h
	ip2c/ip2c.h
	ip2c/ip2cloader.h
	ip2c/ip2cparser.h
	ip2c/ip2cupdater.h
	irc/ircadapterbase.h
	irc/ircchanneladapter.h
	irc/ircclient.h
	irc/ircglobalmessages.h
	irc/ircnetworkadapter.h
	irc/ircprivadapter.h
	irc/ircrequestparser.h
	irc/ircresponseparser.h
	masterserver/masterclient.h
	masterserver/masterclientsignalproxy.h
	masterserver/mastermanager.h
	serverapi/tooltips/gameinfotip.h
	serverapi/tooltips/generalinfotip.h
	serverapi/tooltips/playertable.h
	refresher/refresher.h
	serverapi/binaries.h
	serverapi/message.h
	serverapi/rconprotocol.h
	serverapi/server.h
	updater/autoupdater.h
	updater/updatechannel.h
	updater/updaterinfoparser.h
	updater/updateinstaller.h
	apprunner.h
	customservers.h
	log.h
	main.h
	socketsignalsadapter.h
	connectionhandler.h
)

qt_wrap_cpp(doomseekerWraps QT_FILES ${DOOMSEEKER_QT_FILES})

qt4_add_resources(doomseekerResources
	${DOOMSEEKER_SOURCE_DIR}/media/resources.qrc
)

set(SOURCE_FILES
	configuration/doomseekerconfig.cpp
	configuration/passwordscfg.cpp
	configuration/serverpassword.cpp
	gui/configuration/irc/cfgircappearance.cpp
	gui/configuration/irc/cfgircdefinenetworkdialog.cpp
	gui/configuration/irc/cfgircnetworks.cpp
	gui/configuration/irc/cfgircsounds.cpp
	gui/configuration/irc/ircconfigurationdialog.cpp
	gui/configuration/cfgappearance.cpp
	gui/configuration/cfgautoupdates.cpp
	gui/configuration/cfgcustomservers.cpp
	gui/configuration/cfgfilepaths.cpp
	gui/configuration/cfgip2country.cpp
	gui/configuration/cfgquery.cpp
	gui/configuration/cfgserverpasswords.cpp
	gui/configuration/cfgwadseekerappearance.cpp
	gui/configuration/cfgwadseekergeneral.cpp
	gui/configuration/cfgwadseekeridgames.cpp
	gui/configuration/cfgwadseekersites.cpp
	gui/configuration/configurationdialog.cpp
	gui/configuration/doomseekerconfigurationdialog.cpp
	gui/configuration/engineconfigurationbasebox.cpp
	gui/entity/serverlistfilterinfo.cpp
	gui/helpers/datetablewidgetitem.cpp
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
	gui/widgets/createserverdialogpage.cpp
	gui/widgets/dndfilesystemlistview.cpp
	gui/widgets/fontbutton.cpp
	gui/widgets/importantmessageswidget.cpp
	gui/widgets/memorylineedit.cpp
	gui/widgets/multicombobox.cpp
	gui/widgets/tablewidgetmouseaware.cpp
	gui/widgets/serverconsole.cpp
	gui/widgets/serverfilterbuildermenu.cpp
	gui/widgets/serverlistcontextmenu.cpp
	gui/widgets/serverlistview.cpp
	gui/widgets/serversstatuswidget.cpp
	gui/widgets/wadseekersitestable.cpp
	gui/widgets/wadseekerwadstable.cpp
	gui/aboutdialog.cpp
	gui/commongui.cpp
	gui/copytextdlg.cpp
	gui/createserverdialog.cpp
	gui/demomanager.cpp
	gui/dockBuddiesList.cpp
	gui/ip2cupdatebox.cpp
	gui/logdock.cpp
	gui/mainwindow.cpp
	gui/passwordDlg.cpp
	gui/rconpassworddialog.cpp
	gui/remoteconsole.cpp
	gui/serverfilterdock.cpp
	gui/serverlist.cpp
	gui/standardserverconsole.cpp
	gui/wadseekerinterface.cpp
	ini/inisection.cpp
	ini/inivariable.cpp
	ini/ini.cpp
	ip2c/entities/ip2ccountryinfo.cpp
	ip2c/ip2c.cpp
	ip2c/ip2cloader.cpp
	ip2c/ip2cparser.cpp
	ip2c/ip2cupdater.cpp
	irc/configuration/ircconfig.cpp
	irc/constants/ircresponsetype.cpp
	irc/entities/ircnetworkentity.cpp
	irc/entities/ircresponseparseresult.cpp
	irc/entities/ircuserprefix.cpp
	irc/ircchanneladapter.cpp
	irc/ircchatadapter.cpp
	irc/ircclient.cpp
	irc/ircdelayedoperation.cpp
	irc/ircdelayedoperationlist.cpp
	irc/ircglobal.cpp
	irc/ircglobalmessages.cpp
	irc/ircisupportparser.cpp
	irc/ircmessageclass.cpp
	irc/ircnetworkadapter.cpp
	irc/ircprivadapter.cpp
	irc/ircrequestparser.cpp
	irc/ircresponseparser.cpp
	irc/ircuserinfo.cpp
	irc/ircuserlist.cpp
	masterserver/masterclient.cpp
	masterserver/masterclientsignalproxy.cpp
	masterserver/mastermanager.cpp
	pathfinder/basefileseeker.cpp
	pathfinder/casesensitivefsfileseeker.cpp
	pathfinder/caseinsensitivefsfileseeker.cpp
	pathfinder/filesearchpath.cpp
	plugins/engineplugin.cpp
	plugins/pluginloader.cpp
	refresher/refresher.cpp
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
	serverapi/serversummary.cpp
	serverapi/tooltipgenerator.cpp
	tests/testbase.cpp
	tests/testcore.cpp
	tests/testdatapaths.cpp
	tests/testini.cpp
	tests/testircisupportparser.cpp
	tests/testruns.cpp
	tests/teststrings.cpp
	updater/autoupdater.cpp
	updater/updatechannel.cpp
	updater/updateinstaller.cpp
	updater/updatepackagefilter.cpp
	updater/updaterinfoparser.cpp
	updater/updaterscriptparser.cpp
	apprunner.cpp
	commandline.cpp
	customservers.cpp
	datapaths.cpp
	datastreamoperatorwrapper.cpp
	doomseekerfilepaths.cpp
	fileutils.cpp
	json.cpp
	localization.cpp
	localizationinfo.cpp
	log.cpp
	main.cpp
	pathfinder.cpp
	random.cpp
	scanner.cpp
	speedcalculator.cpp
	strings.cpp
	connectionhandler.cpp
	version.cpp
	versiondump.cpp
)

# For the purpose of generation of proper project files.
set(HEADER_FILES
	${DOOMSEEKER_QT_FILES}
	configuration/doomseekerconfig.h
	configuration/passwordscfg.h
	configuration/serverpassword.h
	gui/entity/serverlistfilterinfo.h
	gui/helpers/datetablewidgetitem.h
	gui/helpers/playersdiagram.h
	gui/irc/ircsounds.h
	gui/models/serverlistproxymodel.h
	gui/widgets/qtabwidgetext.h
	gui/commongui.h
	ini/inisection.h
	ini/inivariable.h
	ip2c/entities/ip2ccountryinfo.h
	irc/configuration/ircconfig.h
	irc/constants/ircresponsetype.h
	irc/entities/ircnetworkentity.h
	irc/entities/ircresponseparseresult.h
	irc/entities/ircuserprefix.h
	irc/ircchatadapter.h
	irc/ircdelayedoperation.h
	irc/ircdelayedoperationlist.h
	irc/ircglobal.h
	irc/ircisupportparser.h
	irc/ircmessageclass.h
	irc/ircnetworkconnectioninfo.h
	irc/ircuserinfo.h
	irc/ircuserlist.h
	pathfinder/basefileseeker.h
	pathfinder/casesensitivefsfileseeker.h
	pathfinder/caseinsensitivefsfileseeker.h
	pathfinder/filesearchpath.h
	plugins/engineplugin.h
	plugins/pluginloader.h
	serverapi/buddyinfo.h
	serverapi/gamerunner.h
	serverapi/player.h
	serverapi/playerslist.h
	serverapi/serverstructs.h
	serverapi/serversummary.h
	serverapi/tooltipgenerator.h
	tests/testbase.h
	tests/testcore.h
	tests/testdatapaths.h
	tests/testini.h
	tests/testircisupportparser.h
	tests/testruns.h
	tests/teststrings.h
	updater/updatepackage.h
	updater/updatepackagefilter.h
	updater/updaterscriptparser.h
	commandline.h
	datapaths.h
	datastreamoperatorwrapper.h
	doomseekerfilepaths.h
	fileutils.h
	global.h
	json.h
	localization.h
	localizationinfo.h
	random.h
	pathfinder.h
	qtmetapointer.h
	scanner.h
	speedcalculator.h
	strings.h
	version.h
	versiondefs.h
	versiondump.h
)

# Translations
set (TRANSLATIONS_FILES
	translations/pl_PL.ts
)

option (UPDATE_TRANSLATIONS "Update source translation translations/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)")
if (UPDATE_TRANSLATIONS)
	set(FILES_TO_TRANSLATE
		${doomseekerUI}
		${SOURCE_FILES}
		${HEADER_FILES}
	)
	qt4_create_translation(TS_FILES 
		${FILES_TO_TRANSLATE}
		${TRANSLATIONS_FILES}
	)
	add_custom_target(translations_target DEPENDS ${TS_FILES})
	message(WARNING 
		"Update source translation translations/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)"
	)
	message(STATUS "make translations_target\n\n")
else (UPDATE_TRANSLATIONS)
	add_custom_target(translations_target)
endif (UPDATE_TRANSLATIONS) 

if (WIN32)
	# Compile&copy Qt translations that need to be redistributed with the .exe.
	# .qm files from these need to be redistributed with the application.
	# This is Windows only.
	# WARNING: This should always be defined after UPDATE_TRANSLATIONS "if".
	set(TRANSLATIONS_FILES
		${TRANSLATIONS_FILES}
		${QT_TRANSLATIONS_DIR}/qt_pl.ts
	)
endif (WIN32)

qt4_add_translation(QM_FILES ${TRANSLATIONS_FILES})

# windows.rc
if(WIN32)
	# compile the Windows resource file.  How we do this depends on the compiler.
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		add_custom_command(
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/windows.o
			COMMAND windres -o ${CMAKE_CURRENT_BINARY_DIR}/windows.o -i ${DOOMSEEKER_SOURCE_DIR}/media/windows.rc
      WORKING_DIRECTORY ${DOOMSEEKER_SOURCE_DIR}/media
			COMMENT "Generating windows resources (windows.rc)"
		)

		set(SOURCE_FILES ${SOURCE_FILES} windows.o)
	else("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(SOURCE_FILES ${SOURCE_FILES} ${DOOMSEEKER_SOURCE_DIR}/media/windows.rc)
	endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
endif(WIN32)

add_executable(doomseeker
	${doomseekerUI}
	${doomseekerResources}
	${QT_FILES}
	${QM_FILES}
	${SOURCE_FILES}
	${HEADER_FILES}
)

# Make sure the svnrevision.h file is created before we attempt to compile
# Doomseeker.
add_dependencies(doomseeker revision_check)

target_link_libraries(doomseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} ${QT_QTXML_LIBRARIES} wadseeker ${CMAKE_DL_LIBS})

# Copy translations
set(TRANSLATION_FILES_TO_COPY
	${QM_FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/translations/translations.def
)

add_custom_command(TARGET doomseeker PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/translations
)

foreach(FILE_TO_COPY ${TRANSLATION_FILES_TO_COPY})
	add_custom_command(TARGET doomseeker POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FILE_TO_COPY} ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/translations
	)
endforeach(FILE_TO_COPY)

# Allow plugins to link with doomseeker
set_target_properties(doomseeker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} ENABLE_EXPORTS 1)
if(WIN32)
	if(MSVC)
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
	else(MSVC)
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_RELEASE "-Wl,-subsystem,windows")
		set_target_properties(doomseeker PROPERTIES LINK_FLAGS_MINSIZEREL "-Wl,-subsystem,windows")
	endif(MSVC)
	
	# Install program DIRECTLY to directory specified by
	# ${CMAKE_INSTALL_PREFIX}. If ${CMAKE_INSTALL_PREFIX} is set to something
	# stupid like "C:\Program Files\" (this is the CMake's default, BTW)
	# then program.exe will be dumped to "C:\Program Files\program.exe"
	install(TARGETS doomseeker RUNTIME DESTINATION .)
	# Install Qt libraries.
	# TODO: This could be rewritten to install the debug libraries
	# when Debug build type is used. On the other hand installing debug builds
	# is rather senseless.
	get_filename_component(QT_DLL_DIR ${QT_QMAKE_EXECUTABLE} PATH)
	set(QT_LIBS_RELEASE
		${QT_DLL_DIR}/QtCore4.dll
		${QT_DLL_DIR}/QtGui4.dll
		${QT_DLL_DIR}/QtNetwork4.dll
		${QT_DLL_DIR}/QtXml4.dll
	)
	install(FILES ${QT_LIBS_RELEASE} DESTINATION .)
	# Install .ico file. No idea what purpose would this server but some
	# programs come with their icons in the .ico file so why not.
	install(FILES ${CMAKE_SOURCE_DIR}/media/icon_windows.ico DESTINATION .
		RENAME doomseeker.ico
	)
	# Install MSVC runtime. Only Visual 2008 is supported at the moment.
	if (MSVC90)
		# Install redist and manifest for the program.
		include("${CMAKE_SOURCE_DIR}/cmake/Modules/FindRequiredSystemLibraries.cmake")
		install_programs(. ${REQUIRED_SYSTEM_LIBRARIES})
		# Install manifest for plugins.
		install(FILES ${CMAKE_SOURCE_DIR}/media/manifest_for_engines_directory/Microsoft.VC90.CRT.manifest
			DESTINATION engines
		)
	endif()
	# Install other files that need to get distributed with binary.
	set(LICENSE_FILES
		${CMAKE_SOURCE_DIR}/LICENSE
		${CMAKE_SOURCE_DIR}/LICENSE.json
		${CMAKE_SOURCE_DIR}/LICENSE.wadseeker
	)
	install(FILES ${TRANSLATION_FILES_TO_COPY} DESTINATION translations)
	install(FILES ${LICENSE_FILES} DESTINATION .)
	install(FILES ${CMAKE_SOURCE_DIR}/tools/doomseeker-portable.bat DESTINATION .)
endif(WIN32)

if(UNIX AND NOT APPLE)
	install(TARGETS doomseeker RUNTIME DESTINATION bin)
	install(FILES ${CMAKE_SOURCE_DIR}/media/icon.png DESTINATION share/doomseeker)
	install(FILES ${TRANSLATION_FILES_TO_COPY} DESTINATION share/doomseeker/translations)
	install(FILES ${CMAKE_SOURCE_DIR}/media/Doomseeker.desktop DESTINATION /usr/share/applications)
endif(UNIX AND NOT APPLE)
