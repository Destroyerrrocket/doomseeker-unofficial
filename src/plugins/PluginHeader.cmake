#------------------------------------------------------------------------------
# PluginHeader.cmake
#------------------------------------------------------------------------------
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301  USA
#------------------------------------------------------------------------------
# Copyright (C) 2010 - 2017 Braden "Blzut3" Obrzut <admin@maniacsvault.net>
#------------------------------------------------------------------------------

# Generic Plugin CMakeList.txt
# Usage:
#  - Set PLUGIN_NAME to the name of your plugin set(PLUGIN_NAME skulltag)
#  - Include this file using include(../PluginHeader.cmake)
#  - add_library(${PLUGIN_NAME} MODULE ${STUFF})
#  - Include the footer file include(../PluginFooter.cmake)

if(MSVC)
	set(CMAKE_SHARED_MODULE_PREFIX "lib")
endif()

if(NOT FORCE_QT4)
	find_package(Qt5 COMPONENTS Widgets)
endif()

if(NOT Qt5_FOUND)
	find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork QtXml REQUIRED)
	set(QT_LIBRARIES Qt4::QtCore Qt4::QtGui Qt4::QtNetwork Qt4::QtXml)
else()
	find_package(Qt5 COMPONENTS Core Gui LinguistTools Multimedia Network Xml REQUIRED)
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
