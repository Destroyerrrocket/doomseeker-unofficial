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
				 * @brief Inits a plugin.
				 *
				 * @param type
				 *     Magic number which is compared with any possible
				 *     plugins to confirm it is the right type.
				 * @param file
				 *     Path to plugin file.
				 */
				Plugin(unsigned int type, QString file);
				virtual ~Plugin();

				/**
				 * @brief Returns a pointer to the requested function or NULL.
				 */
				void	*function(const char* func) const;
				void	initConfig();
				bool	isValid() const;
				/**
				 * @brief Main plugin interface.
				 */
				EnginePlugin *info() const;

			private:
				class PrivData;
				PrivData *d;

				void	unload();
		};

		/**
		 * @brief Gathers information about plugins in a particular directory.
		 */
		PluginLoader(unsigned int type, const QStringList &baseDirectories,
			const QString &subDirectory);
		virtual ~PluginLoader();

		/**
		 * @brief Clears the plugins list
		 */
		void clearPlugins();

		/**
		 * @brief Inits configuration for plugins.
		 */
		void initConfig();

		/**
		 * @brief Gets the number of loaded plugins.
		 *
		 * It will return 0 in safe mode.
		 */
		const unsigned int numPlugins() const;

		/**
		 * @brief Looks for a plugin which name equals to parameter.
		 *
		 * @param name
		 *     Name to look for.
		 * @return index of found plugin in the plugin array, or -1
		 *         if not found.
		 */
		int pluginIndexFromName(const QString& name) const;

		const QList<Plugin*> &plugins() const;

		/**
		 * @brief Resets the plugins directory, clearing the loaded plugins and
		 *        getting new loaded plugins in the process.
		 */
		void resetPluginsDirectory(const QString& pluginsDirectory);

		/**
		 * @brief Returns the requested plugin or NULL.
		 */
		const Plugin* operator[] (unsigned int index) const;

	private:
		class PrivData;
		PrivData *d;

		bool	filesInDir();
};

#endif /* __PLUGINLOADER_HPP__ */
