//------------------------------------------------------------------------------
// serverlistrowhandler.h
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
#ifndef __SERVER_LIST_ROW_HANDLER_H_
#define __SERVER_LIST_ROW_HANDLER_H_

#include <QHostAddress>
#include <QObject>
#include <QStandardItem>
#include <QString>

class GameCVar;
class Server;
class ServerListModel;

class ServerListRowHandler : public QObject
{
	Q_OBJECT

	public:
		/**
		 *	Servers from the same group will be always kept together
		 *  and sorted only inside this group. Group order is always descending:
		 *  SG_NORMAL servers will be always on the top of the list, after them
		 *  will be SG_WAIT servers, etc.
		 *
		 *  @b WARNING:
		 *	Exception: custom servers will always be on top of the list
		 *	and will be sorted inside their own group independentedly.
		 */
		enum ServerGroup
		{
			SGNormal 		= 200,
			SGWait 			= 175,
			SGBanned		= 150,
			SGTimeout		= 125,
			SGBad			= 100,
			SGFirstQuery	= 50,
		};

		enum ServerListDataTypes
		{
			// Pointer to the server structure is always stored in the first column
			DTPointerToServerStructure		= Qt::UserRole+1,
			DTSort							= Qt::UserRole+2
		};

		ServerListRowHandler(ServerListModel* parentModel, int rowIndex, Server* pServer);

		/**
		 *	In this constructor server will be obtained directly from the
		 *	parentModel.
		 */
		ServerListRowHandler(ServerListModel* parentModel, int rowIndex);

		/**
		 *	Removes content from fields for which isColumnVital() returns false.
		 */
		void 					clearNonVitalFields();

		Server*					getServer() { return server; }

		QStandardItem*			item(int columnIndex);

		void					redraw();

		void                    setCountryFlag();
		void 					setRefreshing();

		int 					updateServer(int response);

		static Server* 			serverFromList(ServerListModel* parentModel, int rowIndex);
	protected:
		void 					emptyItem(QStandardItem*);

		void					fillAddressColumn();

		void 					fillItem(QStandardItem*, const QString&);
		void 					fillItem(QStandardItem*, int, const QString&);
		void 					fillItem(QStandardItem*, int);
		void 					fillItem(QStandardItem*, const QHostAddress&, const QString& actualDisplay = QString());
		void 					fillItem(QStandardItem*, const QString&, const QPixmap&);
		void 					fillItem(QStandardItem*, int, const QPixmap&);

		void					fillPlayerColumn();
		void					fillPortIconColumn();
		void					fillServerPointerColumn();

		void 					prepareHeaders();
		//ServerGroup 			serverGroup(int row);


		void					setBackgroundColor();

		void 					setBad();
		void 					setBanned();
		void					setFirstQuery();
		void 					setGood();
		void					setTimeout();
		void 					setWait();

		ServerListModel*		model;
		int						row;
		Server*					server;

	private:
		QStringList extractValidGameCVarNames(const QList<GameCVar> &cvars);
};

#endif
