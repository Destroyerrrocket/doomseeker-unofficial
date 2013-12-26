//------------------------------------------------------------------------------
// pluginloader.h
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

#ifndef __PLUGINLOADER_H__
#define __PLUGINLOADER_H__

#include <QList>
#include <QString>

#include "global.h"

#ifdef Q_OS_WIN32
#include <windows.h>

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif

#endif

class EnginePlugin;

class MAIN_EXPORT PluginLoader
{
	public:
		/**
		* This class handles one specific plugin.  It allows for cross-platform access
		* to the plugins.
		*/
		class MAIN_EXPORT Plugin
		{
			public:
				/**
				* Inits a plugin.  Type is an id which it compares with any possible
				* plugins to confirm it is the right type.
				*/
				Plugin(unsigned int type, QString file);
				~Plugin();

				/**
				* Returns a pointer to the requested function or NULL.
				*/
				void	*function(const char* func) const;

				void	initConfig();

				bool	isValid() const { return library != NULL; }

				EnginePlugin	*info;

			private:
				void	unload();

				QString	file;

		#ifdef Q_OS_WIN32
				HMODULE		library;
		#else
				void		*library;
		#endif
		};

		/**
		 * Gathers information about plugins in a particular directory.
		 * @param directoryLength length of the directory argument.  You do not need to supply if directory is NULL terminated.
		 */
		PluginLoader(unsigned int type, const QStringList &baseDirectories, const char* directory, int directoryLength=-1);
		~PluginLoader();

		/**
		 * Clears the plugins list
		 */
		void clearPlugins();

		/**
		 * Inits configuration for plugins.
		 */
		void initConfig();

		/**
		 * Gets the number of loaded plugins.  It will return 0 in safe mode.
		 */
		const unsigned int numPlugins() const;

		/**
		 *	Looks for a plugin which info::name equals to parameter.
		 * 	@param name - name to look for.
		 *	@return index of found plugin in the plugin array, or -1
		 *		if not found.
		 */
		int pluginIndexFromName(const QString& name) const;

		const QList< Plugin* >& plugins() const
		{
			return pluginsList;
		}

		/**
		 * Resets the plugins directory, clearing the loaded plugins and getting new loaded plugins in the process.
		 */
		void resetPluginsDirectory(const QString& pluginsDirectory);

		/**
		 * Returns the requested plugin or NULL.
		 */
		const Plugin* operator[] (unsigned int index) const;

	private:
		bool	filesInDir();

		unsigned int		type;
		QString				pluginsDirectory;
		QList<Plugin *>		pluginsList;
};

#endif /* __PLUGINLOADER_HPP__ */
