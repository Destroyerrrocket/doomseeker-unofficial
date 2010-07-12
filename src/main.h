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

#include "sdeapi/pluginloader.hpp"
#include "sdeapi/config.hpp"
#include "serverapi/server.h"
#include "datapaths.h"
#include "ip2c.h"
#include "refresher.h"
#include <QApplication>
#include <QObject>
#include <QStringList>

/**
 * This class holds some global information.
 */
class MAIN_EXPORT Main : public QObject
{
	public:
		static Config 				*config;
		static IP2C					*ip2c;
		static DataPaths			dataPaths;
		static QWidget*				mainWindow;
		static PluginLoader*		enginePlugins;
		static bool					running; /// Used to notify the Server objects that it should not refresh in order to end the program faster.
		static RefreshingThread*	refreshingThread;
		static QString				workingDirectory;

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
		void						createMainWindow();
		bool						createRemoteConsole();
		
		int							runTestMode();

		void						initDataDirectories();

		/**
		 *	If updateip2c == true, application should quit after this returns.
		 *	@return If updateip2c == true it returns the exit code. Otherwise
		 *		always returns zero.
		 */
		int							initIP2C();
		void						initMainConfig();
		void						initPluginConfig();

		/**
		 *	@return If false - terminate the application after this method
		 *		returns.
		 */
		bool						interpretCommandLineParameters();
		void						setupRefreshingThread();

		QApplication*				application;
		char**						arguments;
		int							argumentsCount;
		bool						bTestMode;
		QStringList 				dataDirectories;
		QString						rconPluginName; /// If not empty assume we want to launch an rcon client.
		QString						rconAddress;
		unsigned short				rconPort;
		bool						startRcon;
		bool						updateIP2CAndQuit;

};

#endif /* __MAIN_H__ */
