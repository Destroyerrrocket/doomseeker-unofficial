//------------------------------------------------------------------------------
// ircconfig.h
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
#ifndef __IRCCONFIG_H__
#define __IRCCONFIG_H__

#define gIRCConfig IRCConfig::config()

#include "ini.h"
#include "irc/ircnetworkentity.h"


/**
 *	@brief This Singleton holds entire Doomseeker configuration in memory.
 *
 *	The first time it is instantiated the cfg variables are set to default
 *	values. Then they can be either read from or saved to a .ini file.
 *	The save is handled internally and external access to the .ini file
 *	should remain minimal.
 */
class IRCConfig
{
	public:
		class GeneralCfg
		{
			public:
			static const QString		SECTION_NAME;
			
			GeneralCfg();
			
			void						load(IniSection& section);
			void						save(IniSection& section);
			
		};
		
		class PersonalCfg
		{
			public:
			static const QString		SECTION_NAME;
			
			QString						alternativeNickname;
			QString						fullName;
			QString						nickname;
			
		
			PersonalCfg();
			
			void						load(IniSection& section);
			void						save(IniSection& section);
		};
		
		/**
		 *	@brief Complexity of data here requires to create a section
		 *	for each network.
		 *
		 *	Please note that IRCNetworkEntity::description values 
		 *	must be unique.
		 */
		class NetworksDataCfg
		{
			public:
			static const QString		SECTIONS_NAMES_PREFIX;
			
			QVector<IRCNetworkEntity>	networks;
			
			void						networksSortedByDescription(QVector<IRCNetworkEntity>& outVector);
			void						load(Ini& ini);
			void						save(Ini& ini);
			
			private:
				void					clearNetworkSections(Ini& ini);
		};
	
		/**
		 *	@brief Returns the Singleton.
		 */
		static IRCConfig&			config();
		
		/**
		 *	@brief Disposes of the Singleton.
		 *
		 *	This will @b NOT save the current config to a file.
		 *	It is safe to call even if the config was not initialized
		 *	first.
		 */
		static void					dispose();
		
		GeneralCfg					general;
		NetworksDataCfg				networks;
		PersonalCfg					personal;
			
		Ini*						ini() { return this->pIni; }
		
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
		static IRCConfig*			instance;
		
		Ini*						pIni;
	
		IRCConfig();
		~IRCConfig();
};

#endif
