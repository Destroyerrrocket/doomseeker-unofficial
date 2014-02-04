//------------------------------------------------------------------------------
// pluginloader.cpp
//------------------------------------------------------------------------------
//
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//
//------------------------------------------------------------------------------
// Copyright (C) 2011 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "log.h"
#include "configuration/doomseekerconfig.h"
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include "serverapi/masterclient.h"
#include "main.h"
#include "strings.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#define dlopen(a,b)	LoadLibrary(a)
#define dlsym(a,b)	GetProcAddress(a, b)
#define dlclose(a)	FreeLibrary(a)
#define dlerror()	GetLastError()
#else
#include <dlfcn.h>
#include <dirent.h>
#endif

////////////////////////////////////////////////////////////////////////////////
PluginLoader::Plugin::Plugin(unsigned int type, QString f) : file(f), library(NULL)
{
	// Load the library
	library = dlopen(file.toAscii().constData(), RTLD_NOW);

	if(library != NULL)
	{
		unsigned int (*doomSeekerABI)() = (unsigned int(*)()) (dlsym(library, "doomSeekerABI"));
		if(!doomSeekerABI || doomSeekerABI() != DOOMSEEKER_ABI_VERSION)
		{
			// Unsupported version
			unload();
			return;
		}

		EnginePlugin *(*doomSeekerInit)() = (EnginePlugin *(*)()) (dlsym(library, "doomSeekerInit"));
		if(doomSeekerInit == NULL)
		{ // This is not a valid plugin.
			unload();
			return;
		}

		info = doomSeekerInit();
		if(!info->data()->valid)
		{
			unload();
			return;
		}

		gLog << QObject::tr("Loaded plugin: \"%1\"!").arg(info->data()->name);
	}
	else
	{
		gLog << QObject::tr("Failed to open plugin: %1").arg(file);

		gLog << QString("Last error was: %1").arg(dlerror());
	}
}

PluginLoader::Plugin::~Plugin()
{
	unload();
}

void PluginLoader::Plugin::unload()
{
	if(library != NULL)
	{
		dlclose(library);
		library = NULL;
	}
}

void PluginLoader::Plugin::initConfig()
{
	if(library != NULL)
	{
		IniSection cfgSection = gConfig.iniSectionForPlugin(info->data()->name);
		info->setConfig(cfgSection);
	}
}

void *PluginLoader::Plugin::function(const char* func) const
{
	return (void *) dlsym(library, func);
}

////////////////////////////////////////////////////////////////////////////////

PluginLoader::PluginLoader(unsigned int type, const QStringList &baseDirectories, const char* directory, int directoryLength) : type(type)
{
	foreach(QString baseDir, baseDirectories)
	{
		QString subDir = QString::fromAscii(directory, directoryLength != -1 ? directoryLength : static_cast<unsigned int>(-1));
		pluginsDirectory = Strings::combinePaths(baseDir, subDir);
		if(filesInDir())
			break;
	}
	if(numPlugins() == 0) // No plugins?!
		gLog << QObject::tr("Failed to locate plugins.");
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
	gLog << QString("Attempting to load plugins from directory: %1").arg(pluginsDirectory);
#ifdef Q_OS_WIN32
	WIN32_FIND_DATA file;

	// Remember that paths here are also handled by WinAPI functions.
	// It is advisable to convert directory separators to the native '\'
	// separators.
	QString searchPath = Strings::combinePaths(pluginsDirectory, "*.dll");
	searchPath = QDir::toNativeSeparators(searchPath);

	HANDLE directory = FindFirstFile(searchPath.toAscii().constData(), &file);
	if(directory != INVALID_HANDLE_VALUE)
	{
		do {
			if(!(file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				QString pluginFilePath = Strings::combinePaths(pluginsDirectory, file.cFileName);
				pluginFilePath = QDir::toNativeSeparators(pluginFilePath);

				Plugin *plugin = new Plugin(type, pluginFilePath);
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
			QString pluginFilePath = Strings::combinePaths(pluginsDirectory, QString(file->d_name));
			pluginFilePath = QDir::toNativeSeparators(pluginFilePath);

			DIR *temp = opendir(pluginFilePath.toAscii().constData());
			if(temp == NULL) // this is a file
			{
				Plugin *plugin = new Plugin(type, pluginFilePath);
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
		Plugin* plugin = (*it);
		plugin->initConfig();
	}
}

const unsigned int PluginLoader::numPlugins() const
{
	return pluginsList.size();
}

int PluginLoader::pluginIndexFromName(const QString& name) const
{
	// Why the mangling?
	// Ever since version 0.8.1b there was a bug that removed all spacebars
	// from plugin names. This bug is fixed in a commit made on 2013-11-01,
	// but the fix breaks at least some parts of configuration for plugins
	// that have spacebars in their names. For example, all server
	// configurations for Chocolate Doom won't load anymore. To prevent that,
	// we need to treat spacebars as non-existent here. Simply put:
	// "Chocolate Doom" == "ChocolateDoom"
	QString mangledName = QString(name).replace(" ", "");
	for (int i = 0; i < pluginsList.size(); ++i)
	{
		QString mangledCandidate = QString(pluginsList[i]->info->data()->name).replace(" ", "");
		if (mangledName.compare(mangledCandidate) == 0)
		{
			return i;
		}
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

const PluginLoader::Plugin* PluginLoader::operator[] (unsigned int index) const
{
	return pluginsList[index];
}
