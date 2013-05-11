//------------------------------------------------------------------------------
// main.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __MAIN_H__
#define __MAIN_H__

#include "ini/ini.h"
#include "ip2c/ip2c.h"
#include "plugins/pluginloader.h"
#include "datapaths.h"
#include "localizationinfo.h"
#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QUrl>

class Refresher;

/**
 * This class holds some global information.
 */
class MAIN_EXPORT Main : public QObject
{
	public:
		static const QString		IP2C_FILENAME;

		static QApplication*		application;
		/**
		 * @brief If true then program will install updates and restart
		 *        instead of quitting if quit is requested.
		 *
		 * This flag is ignored if the program configuration states that
		 * there are no updates to install.
		 *
		 * Default: false.
		 */
		static bool bInstallUpdatesAndRestart;
		static IP2C					*ip2c;
		static DataPaths*			dataPaths;
		static QWidget*				mainWindow;
		static PluginLoader*		enginePlugins;
		static QList<LocalizationInfo> localizations;
		static bool					running; /// Used to notify the Server objects that it should not refresh in order to end the program faster.
		static Refresher*			refresher;
		static QString				workingDirectory;
		static QString argDataDir;
		static bool bPortableMode;

		Main(int argc, char* argv[]);
		~Main();

		/**
		 *	@brief Called by main() after run() returns.
		 *
		 *	finalize() cleans up after Main object.
		 */
		void						finalize();

		/**
		 *	@brief Replaces main().
		 */
		int							run();

	protected:
		static const QString		DOOMSEEKER_CONFIG_FILENAME;
		static const QString		DOOMSEEKER_INI_FILENAME;
		static const QString		DOOMSEEKER_IRC_INI_FILENAME;

		int							connectToServerByURL();
		void						convertOldIniToQSettingsIni();

		void						createMainWindow();
		bool						createRemoteConsole();

		int							runTestMode();

		/**
		 *	@b Creates required directories and sets up the application.
		 *
		 *	@return False if it was impossible to setup the directories.
		 */
		bool						initDataDirectories();

		/**
		 *	If updateip2c == true, application should quit after this returns.
		 *	@return If updateip2c == true it returns the exit code. Otherwise
		 *		always returns zero.
		 */
		int							initIP2C();
		void						initIRCConfig();
		void initLocalizationsDefinitions();
		void						initMainConfig();
		void						initPluginConfig();
		int installPendingUpdates();

		/**
		 *	@return If false - terminate the application after this method
		 *		returns.
		 */
		bool						interpretCommandLineParameters();
		void						preserveOldConfigBackwardsCompatibility();
		void						setupRefreshingThread();

		char**						arguments;
		int							argumentsCount;
		bool                        bIsFirstRun;
		bool						bTestMode;
		QStringList 				dataDirectories;
		QString						rconPluginName; /// If not empty assume we want to launch an rcon client.
		QString						rconAddress;
		unsigned short				rconPort;
		bool						startRcon;
		int updateFailedCode;
		QUrl						connectUrl;
};

#endif /* __MAIN_H__ */
