#ifndef __SERVERLIST_H_
#define __SERVERLIST_H_

#include <QObject>
#include <QString>

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
			SLDT_POINTER_TO_SERVER_STRUCTURE = Qt::UserRole+1
		};
};

#endif
