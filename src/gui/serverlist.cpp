#include "gui/serverlist.h"

ServerListColumn SLCHandler::columns[] =
{
	{ tr("Players"), 40 },
	{ tr("Ping"), 40 },
	{ tr("Servername"), 200 },
	{ tr("Address"), 150 },
	{ tr("IWAD"), 100 },
	{ tr("MAP"), 80 },
	{ tr("Wads"), 120 },
	{ tr("Gametype"), 150 }
};
