//------------------------------------------------------------------------------
// serverlistrowhandler.cpp
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
#include "serverlistrowhandler.h"
#include "serverlistcolumn.h"
#include "serverlistmodel.h"
#include "configuration/doomseekerconfig.h"
#include "gui/helpers/playersdiagram.h"
#include "gui/widgets/serverlistview.h"
#include "gui/dockBuddiesList.h"
#include "gui/mainwindow.h"
#include "gui/serverlist.h"
#include "ip2c/ip2c.h"
#include "serverapi/playerslist.h"
#include "serverapi/server.h"
#include "serverapi/serverstructs.h"
#include "application.h"
#include "log.h"
#include <QPainter>

using namespace ServerListColumnId;

DClass<ServerListRowHandler>
{
	public:
		ServerPtr server;
};

DPointered(ServerListRowHandler)

ServerListRowHandler::ServerListRowHandler(ServerListModel* parentModel,
    int rowIndex, const ServerPtr &server)
: model(parentModel), row(rowIndex)
{
	d->server = server;
}

ServerListRowHandler::ServerListRowHandler(ServerListModel* parentModel, int rowIndex)
: model(parentModel), row(rowIndex)
{
	d->server = serverFromList(parentModel, rowIndex);
}

ServerListRowHandler::~ServerListRowHandler()
{
}

void ServerListRowHandler::clearNonVitalFields()
{
	for (int i = 0; i < NUM_SERVERLIST_COLUMNS; ++i)
	{
		if(!ServerListColumns::isColumnVital(i))
		{
			emptyItem(item(i));
		}
	}
}

void ServerListRowHandler::emptyItem(QStandardItem* item)
{
	item->setData("", Qt::DisplayRole);
	item->setData(QVariant(), Qt::DecorationRole);
	item->setData(QVariant(), DTSort);
}

QStringList ServerListRowHandler::extractValidGameCVarNames(const QList<GameCVar> &cvars)
{
	QStringList result;
	foreach (const GameCVar &cvar, cvars)
	{
		if (!cvar.isValid())
		{
			result << cvar.name();
		}
	}
	return result;
}

void ServerListRowHandler::fillAddressColumn()
{
	QStandardItem* pItem = item(IDAddress);
	fillItem(pItem, d->server->address(), d->server->hostName() );
}

void ServerListRowHandler::fillItem(QStandardItem* item, const QString& str)
{
	QString strLowcase = str.toLower();
	item->setData(str, Qt::DisplayRole);
	item->setData(strLowcase, DTSort);
}

void ServerListRowHandler::fillItem(QStandardItem* item, int sort, const QString& str)
{
	QVariant var = sort;

	fillItem(item, str);
	item->setData(sort, DTSort);
}

void ServerListRowHandler::fillItem(QStandardItem* item, int num)
{
	QVariant var = num;

	item->setData(var, Qt::DisplayRole);
	item->setData(var, DTSort);
}

void ServerListRowHandler::fillItem(QStandardItem* item, const QHostAddress& addr, const QString& actualDisplay)
{
	QVariant var = addr.toIPv4Address();

	if (actualDisplay.isEmpty())
	{
		item->setData(addr.toString(), Qt::DisplayRole);
	}
	else
	{
		item->setData(actualDisplay, Qt::DisplayRole);
	}
	item->setData(var, DTSort);
}

void ServerListRowHandler::fillItem(QStandardItem* item, const QString& sort, const QPixmap& icon)
{
	item->setIcon(QIcon(icon));
	item->setData(sort, DTSort);
}

void ServerListRowHandler::fillItem(QStandardItem* item, int sort, const QPixmap& image)
{
	item->setData(image, Qt::DecorationRole);
	item->setData(sort, DTSort);
}

void ServerListRowHandler::fillPlayerColumn()
{
	QStandardItem* pItem = item(IDPlayers);

	int style = gConfig.doomseeker.slotStyle;
	bool botsAreNotPlayers = gConfig.doomseeker.bBotsAreNotPlayers;

	const PlayersList &players = d->server->players();
	int sortValue = 0;

	if (botsAreNotPlayers)
	{
		sortValue = players.numClientsWithoutBots();
	}
	else
	{
		sortValue = players.numClients();
	}

	if(style != NUM_SLOTSTYLES)
	{
		fillItem(pItem, sortValue, PlayersDiagram(d->server).pixmap());
	}
	else
	{
		fillItem(pItem, sortValue, QString("%1/%2").arg(players.numClients())
			.arg(d->server->numTotalSlots()));
	}

	// Unset some data if it has been set before.
	pItem->setData(QVariant(QVariant::Invalid), style == NUM_SLOTSTYLES ? Qt::DecorationRole : Qt::DisplayRole);
	pItem->setData(style == NUM_SLOTSTYLES ? 0 : USERROLE_RIGHTALIGNDECORATION, Qt::UserRole);
}

void ServerListRowHandler::fillPortIconColumn()
{
	QStandardItem* pItem = item(IDPort);
	QPixmap icon = d->server->icon();
	if(d->server->isKnown())
	{
		if(d->server->isLockedAnywhere()) // Draw a key if it is locked.
		{
			QPainter iconPainter(&icon);
			iconPainter.drawPixmap(0, 0, QPixmap(":/locked.png"));
			iconPainter.end();
		}
		else if(d->server->isSecure())
		{
			QPainter iconPainter(&icon);
			iconPainter.drawPixmap(0, 0, QPixmap(":/shield.png"));
			iconPainter.end();
		}
	}
	fillItem(pItem, d->server->metaObject()->className(), icon);
}

void ServerListRowHandler::fillServerPointerColumn()
{
	QStandardItem* pItem = item(IDHiddenServerPointer);
	QVariant savePointer = qVariantFromValue(d->server);
	pItem->setData(savePointer, DTPointerToServerStructure);
}

QStandardItem* ServerListRowHandler::item(int columnIndex)
{
	return model->item(row, columnIndex);
}

void ServerListRowHandler::redraw()
{
	updateServer(d->server->lastResponse());

	// Since updateServer doesn't do anything with the flags we need to
	// explicitly redraw it here.
	setCountryFlag();
}

ServerPtr ServerListRowHandler::server()
{
	return d->server;
}

void ServerListRowHandler::setBackgroundColor()
{
	QString color;
	if (d->server->isCustom())
	{
		color = gConfig.doomseeker.customServersColor;
	}
	else if (gApp->mainWindow()->buddiesList()->hasBuddy(d->server))
	{
		if (gConfig.doomseeker.bMarkServersWithBuddies)
		{
			color = gConfig.doomseeker.buddyServersColor;
		}
	}

	for (int column = 0; column < NUM_SERVERLIST_COLUMNS; ++column)
	{
		QBrush brush = !color.isEmpty()
			? QBrush(QColor(color))
			: Qt::NoBrush;
		QStandardItem* pItem = item(column);
		pItem->setBackground(brush);
	}
}

void ServerListRowHandler::setBad()
{
	QStandardItem* qstdItem;

	clearNonVitalFields();

	qstdItem = item(IDServerName);
	fillItem(qstdItem, tr("<ERROR>"));

	qstdItem = item(IDHiddenGroup);
	fillItem(qstdItem, SGBad);
}

void ServerListRowHandler::setBanned()
{
	QStandardItem* qstdItem;

	clearNonVitalFields();

	qstdItem = item(IDServerName);
	fillItem(qstdItem, tr("You are banned from this server!"));

	qstdItem = item(IDHiddenGroup);
	fillItem(qstdItem, SGBanned);
}

void ServerListRowHandler::setCountryFlag()
{
	QStandardItem* pItem = item(IDServerName);

	if (!IP2C::instance()->isDataAccessLocked())
	{
		IP2CCountryInfo countryInfo = IP2C::instance()->obtainCountryInfo(d->server->address());
		if (countryInfo.isValid())
		{
			QPixmap flag = *countryInfo.flag;
			pItem->setIcon(flag);
		}
	}
}

void ServerListRowHandler::setFirstQuery()
{
	QStandardItem* qstdItem = item(IDHiddenGroup);
	fillItem(qstdItem, SGFirstQuery);
}

void ServerListRowHandler::setGood()
{
	QStandardItem* qstdItem;
	QString strTmp;

	fillPlayerColumn();

	qstdItem = item(IDPing);
	fillItem(qstdItem, d->server->ping());

	qstdItem = item(IDServerName);
	fillItem(qstdItem, d->server->name());

	qstdItem = item(IDIwad);
	fillItem(qstdItem, d->server->iwad());

	qstdItem = item(IDMap);
	fillItem(qstdItem, d->server->map());

	strTmp.clear();
	foreach(const PWad &wad, d->server->wads())
	{
		if(wad.isOptional())
		{
			strTmp += QString("[%1] ").arg(wad.name());
		}
		else
		{
			strTmp += wad.name() + " ";
		}
	}
	strTmp.chop(1);
	qstdItem = item(IDWads);
	fillItem(qstdItem, strTmp);

	qstdItem = item(IDGametype);
	QString fullGameModeName = d->server->gameMode().name();
	QStringList modifierNames = extractValidGameCVarNames(d->server->modifiers());
	if (!modifierNames.isEmpty())
	{
		fullGameModeName += QString(" (%1)").arg(modifierNames.join(", "));
	}
	fillItem(qstdItem, fullGameModeName);

	qstdItem = item(IDHiddenGroup);
	fillItem(qstdItem, SGNormal);
}

void ServerListRowHandler::setRefreshing()
{
	QStandardItem* qstdItem = item(IDServerName);
	qstdItem->setText(tr("<REFRESHING>"));
}

void ServerListRowHandler::setTimeout()
{
	QStandardItem* qstdItem;

	clearNonVitalFields();

	qstdItem = item(IDServerName);
	fillItem(qstdItem, tr("<NO RESPONSE>"));

	qstdItem = item(IDHiddenGroup);
	fillItem(qstdItem, SGTimeout);
}

void ServerListRowHandler::setWait()
{
	QStandardItem* qstdItem;

	clearNonVitalFields();

	qstdItem = item(IDServerName);
	fillItem(qstdItem, tr("<Refreshed too soon, wait a while and try again>") );

	qstdItem = item(IDHiddenGroup);
	fillItem(qstdItem, SGWait);
}

ServerPtr ServerListRowHandler::serverFromList(ServerListModel* parentModel, int rowIndex)
{
	QStandardItem* pItem = parentModel->item(rowIndex, IDHiddenServerPointer);
	QVariant pointer = qVariantFromValue(pItem->data(DTPointerToServerStructure));
	if (!pointer.isValid())
	{
		return ServerPtr();
	}
	return pointer.value<ServerPtr>();
}

//ServerListRowHandler::ServerGroup ServerListRowHandler::serverGroup()
//{
//	QStandardItem* qstdItem = item(row, SLCID_HIDDEN_GROUP);
//	return static_cast<ServerListRowHandler::ServerGroup>(qstdItem->data(DTSort).toInt());
//}

int ServerListRowHandler::updateServer(int response)
{
	fillServerPointerColumn();
	fillPortIconColumn();
	fillAddressColumn();

	switch(response)
	{
		case Server::RESPONSE_BAD:
			setBad();
			break;

		case Server::RESPONSE_BANNED:
			setBanned();
			break;

		case Server::RESPONSE_GOOD:
			setGood();
			break;

		case Server::RESPONSE_WAIT:
			if (d->server->isKnown())
			{
				setGood();
			}
			else
			{
				setWait();
			}
			break;

		case Server::RESPONSE_TIMEOUT:
			setTimeout();
			break;

		case Server::RESPONSE_NO_RESPONSE_YET:
			setFirstQuery();
			if (d->server->isRefreshing())
			{
				setRefreshing();
			}
			break;

		default:
			gLog << tr("Unkown server response (%1): %2:%3").arg(response)
				.arg(d->server->address().toString()).arg(d->server->port());
			break;
	}

	setBackgroundColor();

	return row;
}
