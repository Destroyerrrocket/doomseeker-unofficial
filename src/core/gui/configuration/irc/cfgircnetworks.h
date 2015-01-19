//------------------------------------------------------------------------------
// cfgircnetworks.h
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
#ifndef __CFGIRCNETWORKS_H__
#define __CFGIRCNETWORKS_H__

#include "gui/configuration/configurationbasebox.h"
#include <QtContainerFwd>
#include <QIcon>

class IRCNetworkEntity;
class QModelIndex;
class QStandardItem;

class CFGIRCNetworks : public ConfigurationBaseBox
{
	Q_OBJECT

	public:
		CFGIRCNetworks(QWidget* parent = NULL);
		~CFGIRCNetworks();

		QIcon icon() const { return QIcon(":/flags/lan-small"); }
		QString name() const { return tr("Networks"); }
		QVector<IRCNetworkEntity*> networks();
		void readSettings();
		QString title() const { return tr("IRC - Networks"); }

	protected:
		void saveSettings();

	private:
		void addRecord(IRCNetworkEntity* pNetworkEntity);
		void cleanUpTable();
		QList<QStandardItem*> generateTableRecord(IRCNetworkEntity* pNetworkEntity);
		IRCNetworkEntity* network(int row) const;
		QList<IRCNetworkEntity> networksAsQList() const;

		/**
		 *	@brief Never call this function directly. Use network() instead.
		 */
		IRCNetworkEntity* obtainNetworkEntity(QStandardItem* pItem) const;

		void prepareTable();
		void saveNetworks();
		IRCNetworkEntity* selectedNetwork();
		int selectedRow();
		void updateRecord(int row);

		class PrivData;
		PrivData *d;
	private slots:
		void addButtonClicked();
		void editButtonClicked();
		void removeButtonClicked();
		void tableDoubleClicked(const QModelIndex& index);
};

#endif
