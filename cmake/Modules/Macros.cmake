# ------------------------------------------------------------------------------
# Macros.cmake
# ------------------------------------------------------------------------------
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
#  02110-1301  USA
#
# ------------------------------------------------------------------------------
#  Copyright (C) 2017 "Zalewa" <zalewapl@gmail.com>
# ------------------------------------------------------------------------------

# mt/md replacers excellently borrowed from
# http://stackoverflow.com/a/14172871/1089357

set(Macros_CompilerFlags
	CMAKE_CXX_FLAGS
	CMAKE_CXX_FLAGS_DEBUG
	CMAKE_CXX_FLAGS_MINSIZEREL
	CMAKE_CXX_FLAGS_RELEASE
	CMAKE_CXX_FLAGS_RELWITHDEBINFO
	CMAKE_C_FLAGS
	CMAKE_C_FLAGS_DEBUG
	CMAKE_C_FLAGS_MINSIZEREL
	CMAKE_C_FLAGS_RELEASE
	CMAKE_C_FLAGS_RELWITHDEBINFO
)

macro(msvc_mt)
	foreach(CompilerFlag ${Macros_CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
endmacro()

macro(msvc_md)
	foreach(CompilerFlag ${Macros_CompilerFlags})
		string(REPLACE "/MT" "/MD" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
endmacro()

macro(save_flags)
	foreach(CompilerFlag ${Macros_CompilerFlags})
		set(STORE_${CompilerFlag} ${${CompilerFlag}})
	endforeach()
endmacro()

macro(load_flags)
	foreach(CompilerFlag ${Macros_CompilerFlags})
		set(${CompilerFlag} ${STORE_${CompilerFlag}})
	endforeach()
endmacro()
