#ifndef __SERVERLIST_H_
#define __SERVERLIST_H_

#include "server.h"

#include <QObject>
#include <QString>
#include <QTableView>
#include <QStandardItem>

#define HOW_MANY_SERVERLIST_COLUMNS 8

struct ServerListColumn
{
	QString		name;
	int			width;
};

class SLCHandler : public QObject
{
	Q_OBJECT

	public:
		SLCHandler(QTableView*);
		~SLCHandler();

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

	public slots:
		void serverUpdated(const Server *server);

	protected slots:
		// Handles column sorting.
		void columnHeaderClicked(int);

	protected:
		QTableView* table;

		Qt::SortOrder 	sortOrder;
		int				sortIndex;

		void fillItem(QStandardItem*, const QString&);
		void fillItem(QStandardItem*, int);
		void fillItem(QStandardItem*, const QHostAddress&);

		void prepareServerTable();
		QModelIndex findServerOnTheList(const Server* server);
		void addServer(const Server* server);
		void updateServer(const QModelIndex&, const Server* server);
		const Server* serverFromList(int rowNum) const;
        const Server* serverFromList(const QModelIndex&) const;
        const Server* serverFromList(const QStandardItem*) const;
};

#endif
