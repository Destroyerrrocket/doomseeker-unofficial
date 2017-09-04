project(WadseekerApp)

cmake_minimum_required(VERSION 2.8.12)

find_package(Qt4 COMPONENTS QtCore REQUIRED)

find_library(WADSEEKER_LIBRARY wadseeker)
if(WADSEEKER_LIBRARY-NOTFOUND)
	message(SEND_ERROR "Wadseeker was not found.")
else(WADSEEKER_LIBRARY-NOTFOUND)
	message(STATUS "Wadseeker located at ${WADSEEKER_LIBRARY}")
endif()

include_directories(${QT_INCLUDES})

qt4_wrap_cpp(QtMocFiles main.h)
add_executable(wadseeker ${QtMocFiles} main.cpp)

target_link_libraries(wadseeker ${QT_QTCORE_LIBRARY} ${WADSEEKER_LIBRARY})
