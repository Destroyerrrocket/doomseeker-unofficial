//------------------------------------------------------------------------------
// cfgcustomservers.cpp
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
#ifndef __CFGCUSTOMSERVERS_H_
#define __CFGCUSTOMSERVERS_H_

#include "gui/configBase.h"
#include "customservers.h"
#include "ui_cfgcustomservers.h"

class CustomServersConfigBox : public ConfigurationBaseBox, private Ui::CustomServersConfigBox
{
	Q_OBJECT

	public:
		static ConfigurationBoxInfo	*createStructure(Config *cfg, QWidget *parent=NULL);

		void						readSettings();

	protected:
		QStandardItemModel* model;

		CustomServersConfigBox(Config *cfg, QWidget *parent=NULL);


		void	prepareEnginesComboBox();
		void	prepareTable();
		void	saveSettings();
		void	setEngineOnItem(QStandardItem*, const QString& engineName);
		/**
		 *	Encodes table entries into format:
		 * `(<engine_name>;<host_name>;<port>);(...)...`
		 */
		QString	tableEntriesEncoded();

	protected slots:
		void 	add();
		void	add(const QString& engineName, const QString& host, short port);
		void	dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
		void 	remove();
		void 	setEngine();
};

#endif
