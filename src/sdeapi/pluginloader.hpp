// Emacs style mode select   -*- C++ -*-
// =============================================================================
// ### ### ##   ## ###  #   ###  ##   #   #  ##   ## ### ##  ### ###  #  ###
// #    #  # # # # #  # #   #    # # # # # # # # # # #   # #  #   #  # # #  #
// ###  #  #  #  # ###  #   ##   # # # # # # #  #  # ##  # #  #   #  # # ###
//   #  #  #     # #    #   #    # # # # # # #     # #   # #  #   #  # # #  #
// ### ### #     # #    ### ###  ##   #   #  #     # ### ##  ###  #   #  #  #
//                                     --= http://bitowl.com/sde/ =--
// =============================================================================
// Copyright (C) 2008 "Blzut3" (admin@maniacsvault.net)
// The SDE Logo is a trademark of GhostlyDeath (ghostlydeath@gmail.com)
// =============================================================================
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
// =============================================================================
// Description:
// =============================================================================

#ifndef __PLUGINLOADER_HPP__
#define __PLUGINLOADER_HPP__

#include "sdeapi/global.hpp"

#include <string>
#include <vector>

#ifdef WINDOWS
#include <windows.h>

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif

#endif

#define MAKEID(a,b,c,d) (UInt32(a)|(UInt32(b)<<8)|(UInt32(c)<<16)|(UInt32(d)<<24))
/**
 * This is a struct to store information about a specific plugin.  This should
 * returned by the plugins themselves.
 */
struct PluginInfo
{
	public:
		const char*	name;
		const char*	description;
		const char*	author;
		UInt8		version[4];
		UInt32		type; ///< Use MAKEID to generate a check type.
};

/**
 * This class handles one specific plugin.  It allows for cross-platform access
 * to the plugins.
 */
class Plugin
{
	public:
		/**
		 * Inits a plugin.  Type is an id which it compares with any possible
		 * plugins to confirm it is the right type.
		 */
		Plugin(UInt32 type, std::string file);
		~Plugin();

		/**
		 * Returns a pointer to the requested function or NULL.
		 */
		void	*function(const char* func) const;
		bool	isValid() const { return library != NULL; }

		const PluginInfo	*info;

	private:
		void	unload();

		std::string	file;

#ifdef WINDOWS
		HMODULE		library;
#else
		void		*library;
#endif
};

class PluginLoader
{
	public:
		/**
		 * Gathers information about plugins in a particular directory.
		 * @param directoryLength length of the directory argument.  You do not need to supply if directory is NULL terminated.
		 */
		PluginLoader(UInt32 type, const char* directory, Int32 directoryLength=-1);
		~PluginLoader();

		/**
		 * Gets the number of loaded plugins.  It will return 0 in safe mode.
		 */
		const UInt32 numPlugins() const;
		/**
		 * Returns the requested plugin or NULL.
		 */
		const Plugin* operator[] (UInt32 index) const;

	private:
		void	filesInDir();

		UInt32						type;
		std::string					pluginsDirectory;
		std::vector<Plugin *>		pluginsList;
};

#endif /* __PLUGINLOADER_HPP__ */
