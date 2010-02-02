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

#include "log.h"
#include "sdeapi/pluginloader.hpp"

#ifdef Q_OS_WIN32
#include <windows.h>
#define dlopen(a,b)	LoadLibrary(a)
#define dlsym(a,b)	GetProcAddress(a, b)
#define dlclose(a)	FreeLibrary(a)
#else
#include <dlfcn.h>
#include <dirent.h>
#endif

////////////////////////////////////////////////////////////////////////////////
Plugin::Plugin(unsigned int type, QString f) : file(f), library(NULL)
{
	// Load the library
	library = dlopen(file.toAscii().constData(), RTLD_NOW);

	if(library != NULL)
	{
		const PluginInfo *(*doomSeekerInit)() = (const PluginInfo *(*)()) (dlsym(library, "doomSeekerInit"));
		if(doomSeekerInit == NULL)
		{ // This is not a valid plugin.
			unload();
			return;
		}
		info = doomSeekerInit();
		if(info->type != type)
		{ // Make sure this is the right kind of plugin
			unload();
			return;
		}

		pLog << QObject::tr("Loaded plugin: \"%1\"!").arg(info->name);
	}
	else
	{
		pLog << QObject::tr("Failed to open plugin: %1").arg(file);
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

void Plugin::initConfig()
{
	if(library != NULL)
	{
		void (*doomSeekerInitConfig)() = (void (*)()) (dlsym(library, "doomSeekerInitConfig"));
		if(doomSeekerInitConfig != NULL)
		{
			doomSeekerInitConfig();
		}
	}
}

void *Plugin::function(const char* func) const
{
	return (void *) dlsym(library, func);
}

////////////////////////////////////////////////////////////////////////////////

PluginLoader::PluginLoader(unsigned int type, const QStringList &baseDirectories, const char* directory, int directoryLength) : type(type)
{
	foreach(QString baseDir, baseDirectories)
	{
		pluginsDirectory = baseDir + QString::fromAscii(directory, directoryLength != -1 ? directoryLength : static_cast<unsigned int>(-1));
		if(filesInDir())
			break;
	}
	if(numPlugins() == 0) // No plugins?!
		pLog << QObject::tr("Failed to locate plugins.");
}

PluginLoader::~PluginLoader()
{
	for(int i = 0; i < pluginsList.size(); ++i)
	{
		delete pluginsList[i];
	}
}

void PluginLoader::clearPlugins()
{
	for(QList<Plugin *>::iterator iter = pluginsList.begin(); iter != pluginsList.end(); )
	{
		Plugin * plug = (*iter);
		iter = pluginsList.erase(iter);
		delete plug;
		plug = NULL;
	}
}

bool PluginLoader::filesInDir()
{
#ifdef Q_OS_WIN32
	WIN32_FIND_DATA file;
	HANDLE directory = FindFirstFile((pluginsDirectory + "*.dll").toAscii().constData(), &file);
	if(directory != INVALID_HANDLE_VALUE)
	{
		do {
			if(!(file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				Plugin *plugin = new Plugin(type, pluginsDirectory + "/" + file.cFileName);
				if(plugin->isValid())
					pluginsList.push_back(plugin);
				else
					delete plugin;
			}
		} while(FindNextFile(directory, &file));
		FindClose(directory);
	} // WARNING else statement after this
#else
	DIR *directory = opendir(pluginsDirectory.toAscii().constData());
	if(directory != NULL)
	{
		dirent *file = NULL;
		while((file = readdir(directory)) != NULL)
		{
			DIR *temp = opendir((pluginsDirectory + "/" + file->d_name).toAscii().constData());
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
		return false;
	return numPlugins() != 0;
}

void PluginLoader::initConfig()
{
	QList<Plugin*>::iterator it;
	for (it = pluginsList.begin(); it != pluginsList.end(); ++it)
	{
		(*it)->initConfig();
	}
}

const unsigned int PluginLoader::numPlugins() const
{
	return pluginsList.size();
}

int PluginLoader::pluginIndexFromName(const QString& name) const
{
	for (int i = 0; i < pluginsList.size(); ++i)
	{
		if (name.compare(pluginsList[i]->info->name) == 0)
			return i;
	}

	return -1;
}

void PluginLoader::resetPluginsDirectory(const QString& pluginsDirectory)
{
	this->pluginsDirectory = pluginsDirectory;
	if(!pluginsList.isEmpty())
		clearPlugins();
	filesInDir();
}

const Plugin* PluginLoader::operator[] (unsigned int index) const
{
	return pluginsList[index];
}
