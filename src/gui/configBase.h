//------------------------------------------------------------------------------
// configBase.h
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

#ifndef __ENGINE_CONFIG_BASE_H_
#define __ENGINE_CONFIG_BASE_H_

#include "sdeapi/config.hpp"
#include <QGroupBox>
#include <QPushButton>
#include <QStandardItem>


// Base class for configuration group boxes.
// This is the thing selected from the tree view on the left
// of configuration dialog.
class MAIN_EXPORT ConfigurationBaseBox : public QGroupBox
{
	Q_OBJECT;

	public:
		ConfigurationBaseBox(Config* cfg, QWidget* parent = NULL) : QGroupBox(parent)
		{
			bAllowSave = false;
			bSettingsAlreadyRead = false;
			config = cfg;
			hide();
		}

		virtual ~ConfigurationBaseBox() {}

		bool areSettingsAlreadyRead() { return bSettingsAlreadyRead; }

		void setAllowSave(bool b)
		{
			bAllowSave = b;
		}

		void read()
		{
			bSettingsAlreadyRead = true;
			readSettings();
		}

		bool save()
		{
			if (bAllowSave)
			{
				saveSettings();
				return true;
			}
			else
			{
				return false;
			}
		}

	signals:
		/**
		 * This will change default button (the one that is activated when user hits ENTER)
		 * to 'btn'. If NULL is passed as 'btn' argument it will revert to OK button.
		 */
		void 		wantChangeDefaultButton(QPushButton* btn);

	protected:
		Config* 	config;
		bool		bAllowSave;
		bool		bSettingsAlreadyRead;

		/**
		 * These shouldn't execute Config::readConfig() and Config::saveConfig()
		 * methods. They're here to read settings from and write them to controls.
		 */
		virtual void readSettings()=0;

		/**
		 * These shouldn't execute Config::readConfig() and Config::saveConfig()
		 * methods. They're here to read settings from and write them to controls.
		 */
		virtual void saveSettings()=0;

};

// Contains config info for
// particular config box
struct MAIN_EXPORT ConfigurationBoxInfo
{
	public:
		ConfigurationBoxInfo()
		{
			confBox = NULL;
			itemOnTheList = NULL;
		}

		~ConfigurationBoxInfo()
		{
			if (confBox != NULL)
			{
				delete confBox;
			}
		}
		// Name displayed on the engines list.
		QString 				boxName;

		// Index, used by ConfigureDlg class to find.
		QStandardItem*			itemOnTheList;

		ConfigurationBaseBox*	confBox;
};



#endif
