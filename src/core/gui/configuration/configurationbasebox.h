//------------------------------------------------------------------------------
// configurationbasebox.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#ifndef __CONFIGURATION_BASE_BOX_H_
#define __CONFIGURATION_BASE_BOX_H_

#include "ini/ini.h"
#include "global.h"
#include <QPushButton>
#include <QStandardItem>
#include <QWidget>

/**
 *	@brief Base class for configuration group boxes.
 *
 *	Such group boxes contain actual widgets allowing user input
 *	in order to configure program's modules. Settings are being
 *	read() and save()'d through virtual methods, however the
 *	implementation of such methods can do basicaly anything.
 */
class MAIN_EXPORT ConfigurationBaseBox : public QWidget
{
	Q_OBJECT;

	public:
		ConfigurationBaseBox(QWidget* parent = NULL);
		virtual ~ConfigurationBaseBox();

		bool			allowSave();
		bool			areSettingsAlreadyRead();

		/**
		 *	@brief Reimplement this to return displayable icon for the
		 *	ConfigurationBaseBox.
		 *
		 *	If there is no QIcon associated with this box just return
		 *	QIcon object with argument-less constructor.
		 */
		virtual QIcon	icon() const = 0;

		/**
		 * @brief Reimplement this to return list-displayable name for the
		 *        ConfigurationBaseBox.
		 */
		virtual QString	name() const = 0;

		void			setAllowSave(bool b);
		void			read();
		bool			save();
		/**
		 * @brief Groupbox page title, by default returns name().
		 */
		virtual QString title() const;

	signals:
		/**
		 *	This will send a request to the Doomseeker through
		 *	ConfigurationDialog to redraw some graphics.
		 */
		void			appearanceChanged();

	protected:
		/**
		 *	These shouldn't execute Config::readConfig() and
		 *	Config::saveConfig() methods. They're here to read settings
		 *	from and write them to controls.
		 */
		virtual void	readSettings()=0;

		/**
		 *	These shouldn't execute Config::readConfig() and
		 *	Config::saveConfig() methods. They're here to read settings
		 *	from and write them to controls.
		 */
		virtual void	saveSettings()=0;

	private:
		class PrivData;
		PrivData *d;
};

#endif
