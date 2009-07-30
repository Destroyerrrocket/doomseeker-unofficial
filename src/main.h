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
#include "ip2c.h"

#include <QAbstractItemView>
#include <QListView>
#include <QStandardItemModel>

/**
 * This class holds some global information.
 */
class MAIN_EXPORT Main
{
	public:
		static Config 			*config;
		static IP2C				*ip2c;
		static QWidget*			mainWindow;
		static PluginLoader		enginePlugins;
		static bool				running; /// Used to notify the Server objects that it should not refresh in order to end the program faster.

		/**
		 *	Global GUI method.
		 */
		static QStringList		listViewStandardItemsToStringList(QListView* lv)
		{
			QStringList list;
			QStandardItemModel* model = static_cast<QStandardItemModel*>(lv->model());
			for (int i = 0; i < model->rowCount(); ++i)
			{
				list << model->item(i)->text();
			}

			return list;
		}

		/**
		 *	Global GUI method.
		 */
		static void 			removeSelectionFromStandardItemView(QAbstractItemView* view)
		{
			QItemSelectionModel* selModel = view->selectionModel();
			QModelIndexList indexList = selModel->selectedIndexes();
			selModel->clear();

			QStandardItemModel* model = static_cast<QStandardItemModel*>(view->model());
			QList<QStandardItem*> itemList;
			for (int i = 0; i < indexList.count(); ++i)
			{
				itemList << model->itemFromIndex(indexList[i]);
			}

			for (int i = 0; i < itemList.count(); ++i)
			{
				QModelIndex index = model->indexFromItem(itemList[i]);
				model->removeRow(index.row());
			}
		}

		/**
		 *	Translates string in format "hostname:port" to atomic values.
		 *	@param settingValue - string to be translated.
		 *	@param[out] hostname - hostname output.
		 *	@param[out] port - port output.
		 *	@param defaultHostname - default hostname to use if translation fails.
		 *	@param defaultPort - default port to use if translation fails
		 *		or no port is specified.
		 */
		static void				translateServerAddress(const QString& settingValue, QString& hostname, short& port, const QString& defaultHostname, const short defaultPort);

		static bool				isCharOnCharList(char c, const QString& charList);
		static QString&			trim(QString& str, const QString& charList) { return trimr(triml(str, charList), charList); }
		static QString&			trimr(QString& str, const QString& charList);
		static QString&			triml(QString& str, const QString& charList);
};

#endif /* __MAIN_H__ */
