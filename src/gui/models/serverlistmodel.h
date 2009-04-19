#ifndef __SERVER_LIST_MODEL_H_
#define __SERVER_LIST_MODEL_H_

#define HOW_MANY_SERVERLIST_COLUMNS 10

#include <QHostAddress>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include "server.h"

struct ServerListColumn
{
	QString		name;
	int			width;
	bool		bHidden;
};

class ServerListModel : public QStandardItemModel
{
	Q_OBJECT

	friend class SLHandler;

	public:
		enum ColumnId
		{
			SLCID_PLAYERS = 0,
			SLCID_PING = 1,
			SLCID_SERVERNAME = 2,
			SLCID_ADDRESS = 3,
			SLCID_IWAD = 4,
			SLCID_MAP = 5,
			SLCID_WADS = 6,
			SLCID_GAMETYPE = 7,
			SLCID_HIDDEN_GROUP = 8,
			SLCID_HIDDEN_SERVER_POINTER = 9
		};

		/**
		 *	Servers from the same group will be always kept together
		 *  and sorted only inside this group. Group order is always descending:
		 *  SG_CUSTOM servers will be always on the top of the list, after them
		 *  will be SG_NORMAL servers, etc.
		 */
		enum ServerGroup
		{
			SG_CUSTOM 	= 300,
			SG_NORMAL 	= 200,
			SG_BANNED	= 150,
			SG_BAD		= 100
		};

		enum ServerListDataTypes
		{
			// Pointer to the server structure is always stored in the first column
			SLDT_POINTER_TO_SERVER_STRUCTURE = Qt::UserRole+1,
			SLDT_SORT						 = Qt::UserRole+2
		};

		static ServerListColumn columns[];

		ServerListModel(QObject* parent = NULL);

		void destroyRows();

		void fillItem(QStandardItem*, const QString&);
		void fillItem(QStandardItem*, int);
		void fillItem(QStandardItem*, const QHostAddress&, const QString& actualDisplay = QString());

		/**
		 *	Returns row number
		 */
		int addServer(Server* server, int response);

		/**
		 *	Returns row number
		 */
		int updateServer(int row, Server* server, int response);

		void setBad(int row, Server* server);
		void setBanned(int row, Server* server);
		void setGood(int row, Server* server);
		void setTimeout(int row, Server* server);
		void setRefreshing(int row);

		QModelIndex findServerOnTheList(const Server* server);
		Server* serverFromList(int rowNum);
        Server* serverFromList(const QModelIndex&);
        Server* serverFromList(const QStandardItem*);

		void sort( int column, Qt::SortOrder order = Qt::AscendingOrder);

	signals:
		void rowContentChanged(int row);
		void allRowsContentChanged();

	protected:
		void 				clearRows();
		void 				prepareHeaders();
		ServerGroup 		serverGroup(int row);

		/**
		 * Returns:
		 *		0 if rows are equal
		 *	   <0 if row1 is less than row2
		 *	   >0 if row1 is more than row2
		 */
		int					compareColumnSortData(QVariant& var1, QVariant& var2, int column);
		int					compareColumnSortData(int row1, int row2, int column);

		QVariant			columnSortData(int row, int column);
		void 				swapRows(unsigned int row1, unsigned int row2);
};

#endif
