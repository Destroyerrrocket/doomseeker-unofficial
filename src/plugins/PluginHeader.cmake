# Generic Plugin CMakeList.txt
# Usage:
#  - Set PLUGIN_NAME to the name of your plugin set(PLUGIN_NAME skulltag)
#  - Include this file using include(../PluginHeader.cmake)
#  - add_library(${PLUGIN_NAME} MODULE ${STUFF})
#  - Include the footer file include(../PluginFooter.cmake)

cmake_policy(SET CMP0011 OLD) # We want the policy to bleed to the includer.
cmake_policy(SET CMP0020 OLD) # These are DLLs, no need for main() of any kind.

if(MSVC)
	set(CMAKE_SHARED_MODULE_PREFIX "lib")
endif()

if(NOT FORCE_QT4)
	find_package(Qt5Widgets)
endif()

if(NOT Qt5Widgets_FOUND)
	find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork QtXml REQUIRED)
	set(QT_LIBRARIES Qt4::QtCore Qt4::QtGui Qt4::QtNetwork Qt4::QtXml)
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
endif()

message(STATUS "Setting up plugin ${PLUGIN_NAME}. Source dir:\n"
	"    ${CMAKE_CURRENT_SOURCE_DIR}")
