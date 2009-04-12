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


#include "sdeapi/pluginloader.hpp"
#include "sdeapi/global.hpp"

#include <string>
#include <vector>

#ifdef WINDOWS
#include <windows.h>
#define dlopen(a,b)	LoadLibrary(a)
#define dlsym(a,b)	GetProcAddress(a, b)
#define dlclose(a)	FreeLibrary(a)
#else
#include <dlfcn.h>
#include <dirent.h>
#endif

using namespace std;

PluginInfo test = {"Qt Interface", "Provides the standard GUI interface.", "Blzut3", {1,0,0,0}, MAKEID('I','N','T','F')};

Plugin::Plugin(UInt32 type, string file) : file(file), library(NULL)
{
	// Load the library
	library = dlopen(file.c_str(), RTLD_NOW);

	if(library != NULL)
	{
		const PluginInfo *(*SDEInit)() = (const PluginInfo *(*)()) (dlsym(library, "SDEInit"));
		if(SDEInit == NULL)
		{ // This is not a valid SDE plugin.
			unload();
			return;
		}
		info = SDEInit();
		if(info->type != type)
		{ // Make sure this is the right kind of plugin
			unload();
			return;
		}
	}
	else
	{
		printf("Failed to open plugin: %s\n", file.c_str());
	}
}

Plugin::~Plugin()
{
	unload();
}

void Plugin::unload()
{
	if(library != NULL)
	{
		dlclose(library);
		library = NULL;
	}
}

void *Plugin::function(const char* func) const
{
	return (void *) dlsym(library, func);
}

////////////////////////////////////////////////////////////////////////////////

PluginLoader::PluginLoader(UInt32 type, const char* directory, Int32 directoryLength) : type(type)
{
	pluginsDirectory = string(directory, 0, directoryLength != -1 ? directoryLength : string::npos);
	filesInDir();
}

PluginLoader::~PluginLoader()
{
	for(UInt32 i = 0;i < pluginsList.size();i++)
		delete pluginsList[i];
}

void PluginLoader::filesInDir()
{
#ifdef WINDOWS
	WIN32_FIND_DATA file;
	HANDLE directory = FindFirstFile((pluginsDirectory + "*.dll").c_str(), &file);
	if(directory != INVALID_HANDLE_VALUE)
	{
		do {
			if(!(file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				Plugin *plugin = new Plugin(type, pluginsDirectory + "/" + file.cFileName);
				if(plugin->IsValid())
					pluginsList.push_back(plugin);
				else
					delete plugin;
			}
		} while(FindNextFile(directory, &file));
		FindClose(directory);
	} // WARNING else statement after this
#else
	DIR *directory = opendir(pluginsDirectory.c_str());
	if(directory != NULL)
	{
		dirent *file = NULL;
		while((file = readdir(directory)) != NULL)
		{
			DIR *temp = opendir((pluginsDirectory + "/" + file->d_name).c_str());
			if(temp == NULL) // this is a file
			{
				Plugin *plugin = new Plugin(type, pluginsDirectory + "/" + file->d_name);
				if(plugin->isValid())
					pluginsList.push_back(plugin);
				else
					delete plugin;
			}
			else
				closedir(temp);
		}
		closedir(directory);
	} // WARNING else statement after this
#endif
	else
	{ // Error
		printf("Failed to open plugins directory. (%s)\n", pluginsDirectory.c_str());
	}
}

const UInt32 PluginLoader::numPlugins() const
{
	return pluginsList.size();
}

const Plugin* PluginLoader::operator[] (UInt32 index) const
{
	return pluginsList[index];
}
