cmake_minimum_required(VERSION 2.4)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)
include_directories(${QT_INCLUDES})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_ui(doomseekerUI
	gui/mainwindow.ui
	gui/configureDlg.ui
	gui/engineSkulltagConfig.ui
)

qt_wrap_cpp(doomseekerWraps QT_FILES
	gui/configBase.h
	gui/mainwindow.h
	gui/engineSkulltagConfig.h
	gui/configureDlg.h
	gui/serverlist.h
	gui/widgets/serverlistview.h
	odamex/odamexmasterclient.h
	skulltag/skulltagmasterclient.h
	skulltag/skulltagserver.h
	sdeapi/config.hpp
	main.h
	masterclient.h
	server.h
)

add_executable(doomseeker
	${QT_FILES}
	${doomseekerUI}
	gui/configureDlg.cpp
	gui/engineSkulltagConfig.cpp
	gui/mainwindow.cpp
	gui/serverlist.cpp
	gui/widgets/serverlistview.cpp
	huffman/huffman.cpp
	odamex/odamexmasterclient.cpp
	sdeapi/config.cpp
	sdeapi/pluginloader.cpp
	sdeapi/scanner.cpp
	skulltag/skulltagmasterclient.cpp
	skulltag/skulltagserver.cpp
	main.cpp
	masterclient.cpp
	server.cpp
)

target_link_libraries(doomseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})

set_target_properties(doomseeker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
