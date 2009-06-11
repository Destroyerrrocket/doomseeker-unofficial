//------------------------------------------------------------------------------
// configureDlg.h
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

#ifndef __CONFIGUREDIALOG_H_
#define __CONFIGUREDIALOG_H_

#include "gui/configBase.h"
#include "sdeapi/config.hpp"
#include "ui_configureDlg.h"

class ConfigureDlg : public QDialog, private Ui::ConfigureDlg
{
	Q_OBJECT

	public:
		ConfigureDlg(Config* mainCfg, QWidget* parent = NULL);
		~ConfigureDlg();

		bool addEngineConfiguration(ConfigurationBoxInfo*);
		bool customServersChanged() { return bCustomServersChanged; }

	protected slots:
		void btnClicked(QAbstractButton *button);
		void optionListClicked(const QModelIndex&);
		void wantChangeDefaultButton(QPushButton*);

	private:
		Config*		mainConfig;
		bool		bCustomServersChanged;

		// hider for debug purposes, if clicked
		// hides current box and shows nothing
		//QStandardItem*					hider;

		QStandardItem*					enginesRoot;
		QList<ConfigurationBoxInfo*>	configBoxesList;
		QWidget*						currentlyDisplayedCfgBox;
		ConfigurationBaseBox*			customServersCfgBox;

		void 		initOptionsList();
		void		saveSettings();

		/**
		 * Passing NULL as rootItem will add the item to the tree view's root.
		 * Passing <0 to pos parameter will use appendRow method.
		 *
		 * Will return false and do nothing
		 * if the ConfigurationBoxInfo is not properly filled
		 * or if itemOnTheList member is not NULL.
		 */
		bool 					addConfigurationBox(QStandardItem* rootItem, ConfigurationBoxInfo*, int pos = -1);

		void 					showConfigurationBox(QWidget*);
		ConfigurationBoxInfo*	findConfigurationBoxInfo(const QStandardItem*);
};

#endif
