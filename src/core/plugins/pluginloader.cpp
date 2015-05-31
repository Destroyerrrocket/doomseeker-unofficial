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
#include "pluginloader.h"

#include "log.h"
#include "configuration/doomseekerconfig.h"
#include "ini/inisection.h"
#include "ini/inivariable.h"
#include "plugins/engineplugin.h"
#include "serverapi/masterclient.h"
#include "strings.h"
#include <cassert>
#include <QDir>

#ifdef Q_OS_WIN32
	#include <windows.h>
	#define dlopen(a,b)	LoadLibrary(a)
	#define dlsym(a,b)	GetProcAddress(a, b)
	#define dlclose(a)	FreeLibrary(a)
	#define dlerror()	GetLastError()
	#ifdef _MSC_VER
		#pragma warning(disable: 4251)
	#endif
#else
	#include <dlfcn.h>
	#include <dirent.h>
#endif

////////////////////////////////////////////////////////////////////////////////
DClass<PluginLoader::Plugin>
{
	public:
		EnginePlugin *info;
		QString file;
		#ifdef Q_OS_WIN32
			HMODULE library;
		#else
			void *library;
		#endif
};

DPointered(PluginLoader::Plugin)

PluginLoader::Plugin::Plugin(unsigned int type, QString file)
{
	d->file = file;
	// Load the library
	d->library = dlopen(d->file.toUtf8().constData(), RTLD_NOW);

	if(d->library != NULL)
	{
		unsigned int (*doomSeekerABI)() = (unsigned int(*)()) (dlsym(d->library, "doomSeekerABI"));
		if(!doomSeekerABI || doomSeekerABI() != DOOMSEEKER_ABI_VERSION)
		{
			// Unsupported version
			unload();
			return;
		}

		EnginePlugin *(*doomSeekerInit)() = (EnginePlugin *(*)()) (dlsym(d->library, "doomSeekerInit"));
		if(doomSeekerInit == NULL)
		{ // This is not a valid plugin.
			unload();
			return;
		}

		d->info = doomSeekerInit();
		if(!info()->data()->valid)
		{
			unload();
			return;
		}

		gLog << QObject::tr("Loaded plugin: \"%1\"!").arg(info()->data()->name);
		d->info->start();
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

void *PluginLoader::Plugin::function(const char* func) const
{
	return (void *) dlsym(d->library, func);
}

EnginePlugin *PluginLoader::Plugin::info() const
{
	return d->info;
}

void PluginLoader::Plugin::initConfig()
{
	if (isValid())
	{
		IniSection cfgSection = gConfig.iniSectionForPlugin(info()->data()->name);
		info()->setConfig(cfgSection);
	}
}

bool PluginLoader::Plugin::isValid() const
{
	return d->library != NULL;
}

void PluginLoader::Plugin::unload()
{
	if (d->library != NULL)
	{
		dlclose(d->library);
		d->library = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
DClass<PluginLoader>
{
	public:
		unsigned int type;
		QString pluginsDirectory;
		QList<PluginLoader::Plugin*> plugins;
};

DPointered(PluginLoader)

PluginLoader *PluginLoader::staticInstance = NULL;

PluginLoader::PluginLoader(unsigned int type, const QStringList &directories)
{
	d->type = type;
	foreach (const QString &dir, directories)
	{
		d->pluginsDirectory = dir;
		if (filesInDir())
		{
			break;
		}
	}
	if (numPlugins() == 0) // No plugins?!
	{
		gLog << QObject::tr("Failed to locate plugins.");
	}
}

PluginLoader::~PluginLoader()
{
	qDeleteAll(d->plugins);
}

void PluginLoader::clearPlugins()
{
	qDeleteAll(d->plugins);
	d->plugins.clear();
}

void PluginLoader::deinit()
{
	if (staticInstance != NULL)
	{
		delete staticInstance;
		staticInstance = NULL;
	}
}

bool PluginLoader::filesInDir()
{
	gLog << QString("Attempting to load plugins from directory: %1").arg(d->pluginsDirectory);
	QDir dir(d->pluginsDirectory);
	if (!dir.exists())
	{
		return false;
	}
#ifdef Q_OS_WIN32
	QStringList windowsNamesFilter;
	windowsNamesFilter << "*.dll";
	dir.setNameFilters(windowsNamesFilter);
#endif
	foreach (const QString& entry, dir.entryList(QDir::Files))
	{
		QString pluginFilePath = Strings::combinePaths(d->pluginsDirectory, entry);
		Plugin *plugin = new Plugin(d->type, pluginFilePath);
		if (plugin->isValid())
			d->plugins << plugin;
		else
			delete plugin;
	}
	return numPlugins() != 0;
}

EnginePlugin *PluginLoader::info(int pluginIndex) const
{
	const Plugin* p = plugin(pluginIndex);
	if (p != NULL)
	{
		return p->info();
	}
	return NULL;
}

void PluginLoader::init(const QStringList &directories)
{
	if (staticInstance != NULL)
	{
		qDebug() << "Attempting to re-init PluginLoader";
		assert(false);
		return;
	}
	staticInstance = new PluginLoader(MAKEID('E', 'N', 'G', 'N'), directories);
}

void PluginLoader::initConfig()
{
	foreach (Plugin *plugin, d->plugins)
	{
		plugin->initConfig();
	}
}

PluginLoader *PluginLoader::instance()
{
	assert(staticInstance != NULL);
	return staticInstance;
}

const unsigned int PluginLoader::numPlugins() const
{
	return d->plugins.size();
}

const QList<PluginLoader::Plugin*> &PluginLoader::plugins() const
{
	return d->plugins;
}

const PluginLoader::Plugin* PluginLoader::plugin(unsigned int index) const
{
	return d->plugins[index];
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
	for (int i = 0; i < d->plugins.size(); ++i)
	{
		QString mangledCandidate = QString(d->plugins[i]->info()->data()->name).replace(" ", "");
		if (mangledName.compare(mangledCandidate) == 0)
		{
			return i;
		}
	}

	return -1;
}

void PluginLoader::resetPluginsDirectory(const QString& pluginsDirectory)
{
	d->pluginsDirectory = pluginsDirectory;
	clearPlugins();
	filesInDir();
}

const PluginLoader::Plugin* PluginLoader::operator[] (unsigned int index) const
{
	return d->plugins[index];
}
