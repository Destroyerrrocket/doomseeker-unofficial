cmake_minimum_required(VERSION 2.4)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)
include_directories(${QT_INCLUDES})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
	add_definitions(-DMODE_MAIN)
endif(WIN32)

qt4_wrap_ui(doomseekerUI
	gui/aboutDlg.ui
	gui/cfgFilePaths.ui
	gui/configureDlg.ui
	gui/mainwindow.ui
	gui/dockserverinfo.ui
	gui/wadseekerconfig.ui
	gui/wadseekerinterface.ui
)

qt_wrap_cpp(doomseekerWraps QT_FILES
	gui/aboutDlg.h
	gui/cfgFilePaths.h
	gui/configBase.h
	gui/configureDlg.h
	gui/mainwindow.h
	gui/dockserverinfo.h
	gui/serverlist.h
	gui/wadseekerconfig.h
	gui/wadseekerinterface.h
	gui/models/serverlistmodel.h
	gui/widgets/dockedwidgets.h
	gui/widgets/serverlistview.h
	sdeapi/config.hpp
	masterclient.h
	server.h
)

add_executable(doomseeker
	${doomseekerUI}
	${QT_FILES}
	gui/aboutDlg.cpp
	gui/cfgFilePaths.cpp
	gui/configureDlg.cpp
	gui/dockserverinfo.cpp
	gui/mainwindow.cpp
	gui/serverlist.cpp
	gui/wadseekerconfig.cpp
	gui/wadseekerinterface.cpp
	gui/models/serverlistmodel.cpp
	gui/widgets/serverlistview.cpp
	huffman/huffman.cpp
	sdeapi/config.cpp
	sdeapi/pluginloader.cpp
	sdeapi/scanner.cpp
	main.cpp
	masterclient.cpp
	pathfinder.cpp
	server.cpp
)

target_link_libraries(doomseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} wadseeker)

# Allow plugins to link with doomseeker
set_target_properties(doomseeker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR} ENABLE_EXPORTS 1)
if(WIN32)
	set_target_properties(doomseeker PROPERTIES LINK_FLAGS_RELEASE "-Wl,-subsystem,windows")
	set_target_properties(doomseeker PROPERTIES LINK_FLAGS_MINSIZEREL "-Wl,-subsystem,windows")
  set_target_properties(doomseeker PROPERTIES LINK_FLAGS "-Wl,--export-all-symbols")
endif(WIN32)
