//------------------------------------------------------------------------------
// doomseekerconfig.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __DOOMSEEKERCONFIG_H__
#define __DOOMSEEKERCONFIG_H__

#include "customservers.h"
#include "serverapi/buddyinfo.h"
#include <QString>
#include <QStringList>
#include <QVector>

#define gConfig DoomseekerConfig::config()

class Ini;
struct PluginInfo;

/**
 *	@brief This Singleton holds entire Doomseeker configuration in memory.
 *
 *	The first time it is instantiated the cfg variables are set to default
 *	values. Then they can be either read from or saved to a .ini file.
 *	The save is handled internally and external access to the .ini file
 *	should remain minimal.
 */
class DoomseekerConfig
{
	public:
		class DoomseekerCfg
		{
			public:
			static const QString		SECTION_NAME;
			
			
			bool						bBotsAreNotPlayers;
			bool						bCloseToTrayIcon;
			bool						bIP2CountryAutoUpdate;
			bool						bMainWindowMaximized;
			bool						bQueryAutoRefreshDontIfActive;
			bool						bQueryAutoRefreshEnabled;
			bool						bQueryOnStartup;
			bool						bRememberConnectPassword;
			bool						bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn;
			QVector<BuddyInfo>			buddiesList;
			bool						bUseTrayIcon;
			QString						connectPassword;
			QVector<CustomServerInfo>	customServers;
			QString						customServersColor;
			unsigned					ip2CountryDatabaseMaximumAge;
			QString						ip2CountryUrl;
			unsigned					mainWindowWidth;
			unsigned					mainWindowHeight;
			int							mainWindowX;
			int							mainWindowY;
			unsigned					queryAutoRefreshEverySeconds;
			unsigned					queryTimeout;
			unsigned					queryTries;
			QString						previousCreateServerConfigDir;
			QString						previousCreateServerExecDir;
			QString						previousCreateServerWadDir;
			QVector<int>				serverListColumnWidths;
			int							slotStyle;
			QStringList					wadPaths;
			
			DoomseekerCfg();
			
			/**
			 *	@brief Will return false if at least one of them is out of
			 *	bounds.
			 *
			 *	This will ensure that window remains accessible.
			 */
			bool						areMainWindowSizeSettingsValid(int maxValidX, int maxValidY) const;
			
			void						load(IniSection& section);
			void						save(IniSection& section);
			
		};
		
		class WadseekerCfg
		{
			public:
			static const QString		SECTION_NAME;
			
			bool						bSearchInIdgames;
			QString						colorMessageCriticalError;
			QString						colorMessageError;
			QString						colorMessageNotice;
			int							connectTimeoutSeconds;
			int							downloadTimeoutSeconds;
			int							idgamesPriority;
			QString						idgamesURL;
			QStringList					searchURLs;
			QString						targetDirectory;
			
			WadseekerCfg();
			void						load(IniSection& section);
			void						save(IniSection& section);
		};
	
		/**
		 *	@brief Returns the Singleton.
		 */
		static DoomseekerConfig&	config();
		
		/**
		 *	@brief Disposes of the Singleton.
		 *
		 *	This will @b NOT save the current config to a file.
		 *	It is safe to call even if the config was not initialized
		 *	first.
		 */
		static void					dispose();
		
		DoomseekerCfg				doomseeker;
		WadseekerCfg				wadseeker;
			
		Ini*						ini() { return this->pIni; }
		
		/**
		 *	@brief This will assume that the .ini file is initialized.
		 *
		 *	If the pIni pointer is NULL this will call setIniFile() with
		 *	empty string parameter to prevent crash.
		 *
		 *	@param pluginName
		 *		Name of the plugin, equals name of the Ini section stripped
		 *		of all spacebars. Cannot be equal to "wadseeker" or 
		 *		"doomseeker". If invalid pluginName is specified a dummy
		 *		trashcan IniSection is returned and a proper log message
		 *		is generated.
		 *
		 *	@see dummySection
		 */
		IniSection&					iniSectionForPlugin(const QString& pluginName);
		IniSection&					iniSectionForPlugin(const PluginInfo* plugin);
	
		/**
		 *	@brief Reads settings from ini file. This file must be
		 *	previously set by setIniFile() method.
		 */
		bool						readFromFile();
		
		/**
		 *	@brief Saves current settings to ini file. This file must
		 *	be previously set by setIniFile() method.
		 */
		bool						saveToFile();

		/**
		 *	@brief Initializes the Ini class instance to point
		 *	to a file.
		 *
		 *	This method will properly dispose of the previous
		 *	ini file. However it will neither save the old ini file
		 *	or read the new one.
		 */
		bool						setIniFile(const QString& filePath);
		

	private:
		static DoomseekerConfig*	instance;
		
		bool						isValidPluginName(const QString& pluginName) const;
		
		/**
		 *	@brief Exists to allow iniSectionForPlugin() to return
		 *	a trash can if meaningless plugin name is specified.
		 */
		IniSection*					dummySection;
		
		Ini*						pIni;
	
		DoomseekerConfig();
		~DoomseekerConfig();
};

#endif