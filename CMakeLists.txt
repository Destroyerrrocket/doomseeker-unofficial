cmake_minimum_required(VERSION 2.4)
project(DOOMSEEKER)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR} CACHE PATH "Directory in which to build the binary.")

add_subdirectory(src/odamex)
add_subdirectory(src/skulltag)
add_subdirectory(src)
