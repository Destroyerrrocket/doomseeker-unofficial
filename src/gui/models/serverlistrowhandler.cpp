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
#include "gui/helpers/playersdiagram.h"
#include "gui/serverlist.h"
#include "serverapi/playerslist.h"
#include "serverapi/server.h"
#include "ip2c.h"
#include "main.h"
#include "log.h"
#include <QPainter>

using namespace ServerListColumnId;

ServerListRowHandler::ServerListRowHandler(ServerListModel* parentModel, int rowIndex, Server* pServer)
: model(parentModel), row(rowIndex), server(pServer)
{
}

ServerListRowHandler::ServerListRowHandler(ServerListModel* parentModel, int rowIndex)
: model(parentModel), row(rowIndex)
{
	server = serverFromList();
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
	item->setData(QVariant(), DTSort);
}

void ServerListRowHandler::fillAddressColumn()
{
	QStandardItem* pItem = item(IDAddress);
	fillItem(pItem, server->address(), QString(server->address().toString() + ":" + QString::number(server->port())) );
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
	IniSection& config = model->handler()->configurationObject();
	QStandardItem* pItem = item(IDPlayers);

	int style = config["SlotStyle"];
	bool botsAreNotPlayers = config["BotsAreNotPlayers"];

	const PlayersList* playersList = server->playersList();
	int sortValue = 0;

	if (botsAreNotPlayers)
	{
		sortValue = playersList->numClientsWithoutBots();
	}
	else
	{
		sortValue = playersList->numClients();
	}

	if(style != NUM_SLOTSTYLES)
	{
		fillItem(pItem, sortValue, PlayersDiagram(server).pixmap());
	}
	else
	{
		fillItem(pItem, sortValue, QString("%1/%2").arg(playersList->numClients()).arg(server->maximumClients()));
	}

	// Unset some data if it has been set before.
	pItem->setData(QVariant(QVariant::Invalid), style == NUM_SLOTSTYLES ? Qt::DecorationRole : Qt::DisplayRole);
	pItem->setData(style == NUM_SLOTSTYLES ? 0 : USERROLE_RIGHTALIGNDECORATION, Qt::UserRole);
}

void ServerListRowHandler::fillPortIconColumn()
{
	QStandardItem* pItem = item(IDPort);
	QPixmap icon = server->icon();
	if(server->isLocked() && server->isKnown()) // Draw a key if it is locked.
	{
		QPainter iconPainter(&icon);
		iconPainter.drawPixmap(0, 0, QPixmap(":/locked.png"));
		iconPainter.end();
	}
	fillItem(pItem, server->metaObject()->className(), icon);
}

void ServerListRowHandler::fillServerPointerColumn()
{
	QStandardItem* pItem = item(IDHiddenServerPointer);
	ServerPointer ptr(server);
	QVariant savePointer = qVariantFromValue(ptr);
	pItem->setData(savePointer, DTPointerToServerStructure);
}

QStandardItem* ServerListRowHandler::item(int columnIndex)
{
	return model->item(row, columnIndex);
}

void ServerListRowHandler::redraw()
{
	updateServer(server->lastResponse());

	// Since updateServer doesn't do anything with the flags we need to
	// explicitly redraw it here.
	setCountryFlag();
}

void ServerListRowHandler::setBackgroundColor()
{
	if (server->isCustom())
	{
		QString color = model->handler()->configurationObject()["CustomServersColor"];

		for (int column = 0; column < NUM_SERVERLIST_COLUMNS; ++column)
		{
			QStandardItem* pItem = item(column);
			pItem->setBackground( QBrush(QColor(color )) );
		}
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

	if (!Main::ip2c->isDataAccessLocked())
	{
		CountryInfo countryInfo = Main::ip2c->obtainCountryInfo(server->address());
		if (countryInfo.isFlagOk())
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
	fillItem(qstdItem, server->ping());

	qstdItem = item(IDServerName);
	fillItem(qstdItem, server->name());

	qstdItem = item(IDIwad);
	fillItem(qstdItem, server->iwadName());

	qstdItem = item(IDMap);
	fillItem(qstdItem, server->map());

	strTmp.clear();
	foreach(const PWad &wad, server->pwads())
	{
		if(wad.optional)
			strTmp += QString("[%1] ").arg(wad.name);
		else
			strTmp += wad.name + " ";
	}
	strTmp.chop(1);
	qstdItem = item(IDWads);
	fillItem(qstdItem, strTmp);

	qstdItem = item(IDGametype);
	const GameCVar *modifier = server->modifier();
	fillItem(qstdItem, server->gameMode().name() + (modifier == NULL ? "" : " (" + modifier->name + ")"));

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

Server* ServerListRowHandler::serverFromList()
{
	QStandardItem* pItem = item(IDHiddenServerPointer);
	QVariant pointer = qVariantFromValue(pItem->data(DTPointerToServerStructure));
    if (!pointer.isValid())
    {
        return NULL;
    }
    ServerPointer savedServ = qVariantValue<ServerPointer>(pointer);
    return savedServ.ptr;
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
			if (server->isKnown())
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
			break;

		default:
			gLog << tr("Unkown server response (%1): %2:%3").arg(response).arg(server->address().toString()).arg(server->port());
			break;
	}

	setBackgroundColor();

	return row;
}

//QVariant ServerListRowHandler::columnSortData(int row, int column)
//{
//	QStandardItem* it = item(row, column);
//	return it->data(DTSort);
//}

//void ServerListRowHandler::updateFlag(int row, bool force)
//{
//    Server* serv = serverFromList(row);
//    QStandardItem* itm = item(row, SLCID_SERVERNAME);
//
//    if (force || itm->icon().isNull())
//    {
//		setCountryFlag(itm, serv->address());
//    }
//}
