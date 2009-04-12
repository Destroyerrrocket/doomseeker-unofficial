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

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include "sdeapi/global.hpp"

#include <QString>
#include <map>

struct SettingsData : public QObject
{
	public:
		enum SettingType
		{
			ST_INT,
			ST_STR
		};

		SettingsData(Int32 integer=0) : m_integer(0), m_str(tr("")), m_type(ST_INT) { setValue(integer); }
		SettingsData(QString str) : m_integer(0), m_str(tr("")), m_type(ST_STR) { setValue(str); }

		const Int32			integer() { return m_integer; }
		const QString		string()	{ return m_str; }
		const SettingType	type() { return m_type; }
		void				setValue(Int32 integer) { this->m_integer = integer;this->m_type = ST_INT; }
		void				setValue(QString str) { this->m_str = str;this->m_type = ST_STR; }

	protected:
		SettingType			m_type;
		Int32				m_integer;
		QString				m_str;
};

class Config : public QObject
{
	public:
		Config();
		~Config();

		/**
		 * Creates the specified setting if it hasn't been made already.  It
		 * will be set to the default value.
		 */
		void			createSetting(const QString index, UInt32 defaultInt);
		void			createSetting(const QString index, QString defaultString);
		/**
		 * Gets the specified setting.  Will return NULL if the setting does
		 * not exist.
		 */
		SettingsData*	setting(const QString index);
		/**
		 * Returns if this is an entirely new configuration file.  This can be
		 * used to see if a first time set up wizard should be run.
		 */
		bool			isNewConfig() { return firstRun; }
		/**
		 * Looks for the config file and creates the directory if needed.  This
		 * will call ReadConfig if there is a file to be read.
		 * @see SaveConfig
		 * @see ReadConfig
		 */
		void			locateConfigFile(int argc, char* argv[]);
		/**
		 * Reads the configuration file for settings.  This is ~/.sde/sde.cfg
		 * on unix systems.
		 * @see LocateConfigFile
		 * @see SaveConfig
		 */
		void			readConfig();
		/**
		 * Saves the configuration to a file.  ~/.sde/sde.cfg on unix systems.
		 * @see LocateConfigFile
		 * @see ReadConfig
		 */
		void			saveConfig();

		/**
		 * Converts str into a form that can be stored into config files.
		 */
		static const QString&	escape(QString& str);
	protected:
		bool			findIndex(const QString index, SettingsData *&data);

		bool								firstRun;
		QString								configFile;
		std::map<QString, SettingsData *>	settings;
};

#endif /* __CONFIG_HPP__ */
