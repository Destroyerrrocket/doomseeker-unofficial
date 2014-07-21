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

#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QUrl>

class LocalizationInfo;

/**
 * This class holds some global information.
 */
class Main : public QObject
{
	Q_OBJECT

	public:
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
		static QList<LocalizationInfo> localizations;
		static QString argDataDir;

		Main(int argc, char* argv[]);
		~Main();

		/**
		 *	@brief Called by main() after run() returns.
		 *
		 *	finalize() cleans up after Main object.
		 */
		void finalize();

		/**
		 *	@brief Replaces main().
		 */
		int run();

	protected:
		int connectToServerByURL();
		void convertOldIniToQSettingsIni();

		void createMainWindow();
		bool createRemoteConsole();

		int runTestMode();

		/**
		 *	@b Creates required directories and sets up the application.
		 *
		 *	@return False if it was impossible to setup the directories.
		 */
		bool initDataDirectories();

		/**
		 *	If updateip2c == true, application should quit after this returns.
		 *	@return If updateip2c == true it returns the exit code. Otherwise
		 *		always returns zero.
		 */
		int initIP2C();
		void initIRCConfig();
		void initLocalizationsDefinitions();
		void initMainConfig();
		void initPasswordsConfig();
		void initPluginConfig();
		int installPendingUpdates();

		/**
		 *	@return If false - terminate the application after this method
		 *		returns.
		 */
		bool interpretCommandLineParameters();
		void setupRefreshingThread();

		char** arguments;
		int argumentsCount;
		bool bIsFirstRun;
		bool bTestMode;
		QStringList dataDirectories;
		QString rconPluginName; /// If not empty assume we want to launch an rcon client.
		QString rconAddress;
		unsigned short rconPort;
		bool startRcon;
		int updateFailedCode;
		QUrl connectUrl;

	private:
		bool bPortableMode;
};

#endif /* __MAIN_H__ */
