project(WadseekerApp)

cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(Qt4 COMPONENTS QtCore REQUIRED)

find_library(WADSEEKER_LIBRARY wadseeker)
if(WADSEEKER_LIBRARY-NOTFOUND)
	message(SEND_ERROR "Wadseeker was not found.")
else(WADSEEKER_LIBRARY-NOTFOUND)
	message(STATUS "Wadseeker located at ${WADSEEKER_LIBRARY}")
endif(WADSEEKER_LIBRARY-NOTFOUND)

include_directories(${QT_INCLUDES})

qt4_wrap_cpp(QtMocFiles main.h)
add_executable(wadseeker ${QtMocFiles} main.cpp)

target_link_libraries(wadseeker ${QT_QTCORE_LIBRARY} ${WADSEEKER_LIBRARY})
