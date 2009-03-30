cmake_minimum_required(VERSION 2.4)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

include_directories(${DOOMSEEKER_SOURCE_DIR}/src)
include_directories(${QT_INCLUDES})

qt_wrap_cpp(doomseekerWraps QT_FILES
	odamex/odamexmasterclient.h
	skulltag/skulltagmasterclient.h
	skulltag/skulltagserver.h
	main.h
	masterclient.h
	server.h
)

add_executable(doomseeker
	${QT_FILES}
	huffman/huffman.cpp
	odamex/odamexmasterclient.cpp
	skulltag/skulltagmasterclient.cpp
	skulltag/skulltagserver.cpp
	main.cpp
	masterclient.cpp
	server.cpp
)

target_link_libraries(doomseeker ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})

set_target_properties(doomseeker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
