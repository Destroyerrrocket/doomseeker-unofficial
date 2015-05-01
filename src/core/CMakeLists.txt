cmake_minimum_required(VERSION 2.8.7)

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
endif()

# Get the SVN revision
add_custom_target(revision_check ALL
	COMMAND updaterevision . ${CMAKE_CURRENT_SOURCE_DIR}/svnrevision.h
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS updaterevision
)

find_package(Qt5Widgets)
if(NOT Qt5Widgets_FOUND)
	find_package(Qt4 REQUIRED)
	include(${QT_USE_FILE})
else()
	find_package(Qt5Core REQUIRED)
	find_package(Qt5Gui REQUIRED)
	find_package(Qt5LinguistTools REQUIRED)
	find_package(Qt5Multimedia REQUIRED)
	find_package(Qt5Network REQUIRED)
	find_package(Qt5Xml REQUIRED)
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
	set(QT_LIBRARIES Qt5::Widgets Qt5::Multimedia Qt5::Network Qt5::Xml Qt5::Gui Qt5::Core)
	set(QT_INCLUDES ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Multimedia_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS})
	add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Multimedia_DEFINITIONS} ${Qt5Network_DEFINITIONS} ${Qt5Xml_DEFINTIONS})
	get_target_property(QMAKE_EXEC Qt5::qmake LOCATION)
endif()
find_package(ZLIB REQUIRED)

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
endif()

if(WITH_AUTOUPDATES)
	add_definitions(-DWITH_AUTOUPDATES)
endif()

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

add_definitions("-DINSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

include("CMakeSpawnFileListing.txt")
# This hack allows CMake to regenerate the project when spawned
# file listing file changes from outside sources. This covers
# updating local repo and checking out different revisions.
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/CMakeFileListing.txt"
	"${CMAKE_CURRENT_SOURCE_DIR}/CMakeFileListing.txt"
	NEWLINE_STYLE LF
)
include("CMakeFileListing.txt")

qt4_wrap_ui(doomseekerUI ${UI_FILES})

qt4_add_resources(doomseekerResources
	${DOOMSEEKER_SOURCE_DIR}/media/resources.qrc
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
else ()
	add_custom_target(translations_target)
endif ()

if (WIN32 AND NOT Qt5Widgets_FOUND)
	# Compile&copy Qt4 translations that need to be redistributed with the .exe.
	# .qm files from these need to be redistributed with the application.
	# WARNING: This should always be defined after UPDATE_TRANSLATIONS "if".
	set(TRANSLATIONS_FILES
		${TRANSLATIONS_FILES}
		${QT_TRANSLATIONS_DIR}/qt_pl.ts)
endif()

qt4_add_translation(QM_FILES ${TRANSLATIONS_FILES})

if (WIN32 AND Qt5Widgets_FOUND)
	# Copy Qt5 translations that need to be redistributed with the .exe.
	# WARNING: This should always be defined after qt4_add_translation.
	execute_process(COMMAND ${QMAKE_EXEC} -query QT_INSTALL_TRANSLATIONS
		OUTPUT_VARIABLE QT_TRANSLATIONS_DIR
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(QM_FILES
		${QM_FILES}
		${QT_TRANSLATIONS_DIR}/qt_pl.qm)
endif ()

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
	else()
		set(SOURCE_FILES ${SOURCE_FILES} ${DOOMSEEKER_SOURCE_DIR}/media/windows.rc)
	endif()
endif()

add_executable(doomseeker
	${doomseekerUI}
	${doomseekerResources}
	${QM_FILES}
	${UI_FILES}
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

set_target_properties(doomseeker PROPERTIES AUTOMOC 1)
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
	endif()

	# Install program DIRECTLY to directory specified by
	# ${CMAKE_INSTALL_PREFIX}. If ${CMAKE_INSTALL_PREFIX} is set to something
	# stupid like "C:\Program Files\" (this is the CMake's default, BTW)
	# then program.exe will be dumped to "C:\Program Files\program.exe"
	install(TARGETS doomseeker RUNTIME DESTINATION .)
	# Install Qt libraries.
	if (Qt5Widgets_FOUND)
		# Qt5*.dll
		foreach(qtlib ${QT_LIBRARIES})
			get_target_property(qtlib_location ${qtlib} LOCATION)
			install(FILES ${qtlib_location} DESTINATION .)
		endforeach()
		# plugins
		execute_process(COMMAND ${QMAKE_EXEC} -query QT_INSTALL_PLUGINS
			OUTPUT_VARIABLE QT_PLUGINS_DIR
			OUTPUT_STRIP_TRAILING_WHITESPACE)
		install(FILES ${QT_PLUGINS_DIR}/platforms/qwindows.dll DESTINATION platforms)
		# ICU (the 21 Megabyte bastard)
		execute_process(COMMAND ${QMAKE_EXEC} -query QT_INSTALL_LIBEXECS
			OUTPUT_VARIABLE QT_LIBEXECS_DIR
			OUTPUT_STRIP_TRAILING_WHITESPACE)
		install(FILES
			${QT_LIBEXECS_DIR}/icuin53.dll
			${QT_LIBEXECS_DIR}/icuuc53.dll
			${QT_LIBEXECS_DIR}/icudt53.dll
			DESTINATION .)
	else()
		get_filename_component(QT_DLL_DIR ${QT_QMAKE_EXECUTABLE} PATH)
		set(QT_LIBS_RELEASE
			${QT_DLL_DIR}/QtCore4.dll
			${QT_DLL_DIR}/QtGui4.dll
			${QT_DLL_DIR}/QtNetwork4.dll
			${QT_DLL_DIR}/QtXml4.dll
			)
		install(FILES ${QT_LIBS_RELEASE} DESTINATION .)
	endif()
	# Install .ico file. No idea what purpose would this server but some
	# programs come with their icons in the .ico file so why not.
	install(FILES ${CMAKE_SOURCE_DIR}/media/icon_windows.ico DESTINATION .
		RENAME doomseeker.ico
	)
	# Install MSVC runtime. Only Visual 2008 and 2013 are supported at the moment.
	if (MSVC90 OR MSVC12)
		# Install redist and manifest for the program.
		include("${CMAKE_SOURCE_DIR}/cmake/Modules/FindRequiredSystemLibraries.cmake")
		install_programs(. ${REQUIRED_SYSTEM_LIBRARIES})
	endif()
	if (MSVC90)
		# Install manifest for plugins.
		install(FILES ${CMAKE_SOURCE_DIR}/media/manifest_for_engines_directory/Microsoft.VC90.CRT.manifest
			DESTINATION engines)
	endif()

	# License files.
	# Text files in Windows distribution really should have a .txt extension
	# so rename them all accordingly.
	set(LICENSE_FILES
		LICENSE
		LICENSE.json
		LICENSE.wadseeker
	)
	foreach (LICFILE ${LICENSE_FILES})
		install(FILES "${CMAKE_SOURCE_DIR}/${LICFILE}" DESTINATION .
			RENAME "${LICFILE}.txt")
	endforeach(LICFILE)

	# Translatons
	install(FILES ${TRANSLATION_FILES_TO_COPY} DESTINATION translations)
	# Other
	install(FILES ${CMAKE_SOURCE_DIR}/tools/doomseeker-portable.bat DESTINATION .)
endif()

if(UNIX AND NOT APPLE)
	install(TARGETS doomseeker RUNTIME DESTINATION bin)
	install(FILES ${CMAKE_SOURCE_DIR}/media/icon.png DESTINATION share/doomseeker)
	install(FILES ${TRANSLATION_FILES_TO_COPY} DESTINATION share/doomseeker/translations)
	install(FILES ${CMAKE_SOURCE_DIR}/media/Doomseeker.desktop DESTINATION /usr/share/applications)
endif()
