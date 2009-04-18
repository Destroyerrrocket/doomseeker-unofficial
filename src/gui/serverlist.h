#ifndef __SERVERLIST_H_
#define __SERVERLIST_H_

#include "server.h"

#include <QObject>
#include <QString>
#include <QTableView>
#include <QStandardItem>
#include "masterclient.h"

#define HOW_MANY_SERVERLIST_COLUMNS 8
#define TAB_WIDTH					8

struct ServerListColumn
{
	QString		name;
	int			width;
};

class SLHandler : public QObject
{
	Q_OBJECT

	public:
		SLHandler(QTableView*);
		~SLHandler();

		void clearTable();

		static ServerListColumn columns[];

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
		};

		enum ServerListDataTypes
		{
			// Pointer to the server structure is always stored in the first column
			SLDT_POINTER_TO_SERVER_STRUCTURE = Qt::UserRole+1,
			SLDT_SORT						 = Qt::UserRole+2
		};

		void 			setMaster(MasterClient*);
		QList<Server*>*	serverList()
		{
			if (master == NULL)
			{
				return NULL;
			}

			return &master->serverList();
		}

	public slots:
		void serverUpdated(Server *server, int response);
		void refreshAll();
		void tableRightClicked(const QModelIndex&);

	protected slots:
		// Handles column sorting.
		void columnHeaderClicked(int);

	protected:
		QTableView* 	table;
		MasterClient*	master;

		Qt::SortOrder 	sortOrder;
		int				sortIndex;

		void fillItem(QStandardItem*, const QString&);
		void fillItem(QStandardItem*, int);
		void fillItem(QStandardItem*, const QHostAddress&);

		void prepareServerTable();
		QModelIndex findServerOnTheList(const Server* server);

		void addServer(Server* server);
		void updateServer(int row, Server* server);
		void setRefreshing(int row);

		QString createPlayersToolTip(const Server* server);

		Server* serverFromList(int rowNum);
        Server* serverFromList(const QModelIndex&);
        Server* serverFromList(const QStandardItem*);

		QString spawnGeneralInfoTable(const Server* server);
		QString spawnPlayerTable(const Server* server);
		/**
		 * This will return absolutely nothing if the list is empty.
		 */
        QString spawnPartOfPlayerTable(QList<const Player*>, QString status, int colspan, bool isTeamgame, bool bAppendEmptyRowAtBeginning);
};

#endif
