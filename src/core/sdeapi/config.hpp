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

#include <QString>
#include <QHash>

#include "global.h"

class MAIN_EXPORT Config;

class MAIN_EXPORT SettingsData : public QObject
{
	Q_OBJECT

	public:
		enum SettingType
		{
			ST_INT,
			ST_STR
		};

		bool				boolean() const { return static_cast<bool>(m_integer); }
		const int			integer() const { return m_type == ST_INT ? m_integer : m_str.toInt(); }
		const QString		string() const { return m_type == ST_STR ? m_str : QString::number(m_integer); }
		const SettingType	type() const { return m_type; }

		/**
		 *	@brief Directly translated to integer. 1 for true, 0 for false.
		 */
		void				setValueBoolean(bool b) { setValue(b ? 1 : 0); }
		void				setValue(int integer);
		void				setValue(QString str);

	protected:
		SettingType	m_type;
		int			m_integer;
		QString		m_str;
		Config*		m_parent;

		SettingsData(Config* parent, int integer=0) : m_type(ST_INT), m_integer(0), m_str(tr("")), m_parent(parent) { setValue(integer); }
		SettingsData(Config* parent, QString str) : m_type(ST_STR), m_integer(0), m_str(tr("")), m_parent(parent) { setValue(str); }

	friend class Config;
};

class MAIN_EXPORT Config : public QObject
{
	Q_OBJECT

	public:
		Config();

		/**
		 *	Allows specifying a custom path to the config file.
		 */
		Config(const QString& cfgFile, bool readCfg = true);
		~Config();

		/**
		 *	Erases all settings.
		 */
		void			clear();

		/**
		 * Creates the specified setting if it hasn't been made already.  It
		 * will be set to the default value.
		 */
		void			createSetting(const QString index, unsigned int defaultInt);
		void			createSetting(const QString index, QString defaultString);

		/**
		 * Get all of the settings.  This is here since we're moving away from
		 * this Config class to the Ini class.
		 */
		const QHash<QString, SettingsData *> getSettings() { return settings; }

		void			remove();

		/**
		 * Gets the specified setting.  Will create the setting if it does not
		 * exist.
		 */
		SettingsData*	setting(const QString index);

		/**
		 * @return - true if setting exists in the config file, false if not
		 */
		bool			settingExists(const QString& index);

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
		void			locateConfigFile(const QString& fileName);
		/**
		 * Reads the configuration file for settings.  This is ~/.doomseeker/doomseeker.cfg
		 * on unix systems.
		 * @see LocateConfigFile
		 * @see SaveConfig
		 */
		void			readConfig();

		/**
		 * Converts str into a form that can be stored into config files.
		 */
		static const QString&	escape(QString& str);
		static const QString&	unescape(QString& str);

	public slots:
		/**
		 * Saves the configuration to a file.  (default: ~/.doomseeker/doomseeker.cfg on unix systems)
		 * @see LocateConfigFile
		 * @see ReadConfig
		 */
		bool			saveConfig();

	protected:
		bool			findIndex(const QString index, SettingsData *&data);

		bool							firstRun;
		QString							configFile;
		QHash<QString, SettingsData *>	settings;

		/**
		 * Stores if there has been a change to the configs.  This is used to
		 * save a write cycle if nothing has changed during the program
		 * operation.
		 */
		bool							settingsChanged;

	friend class SettingsData;
};

#endif /* __CONFIG_HPP__ */
