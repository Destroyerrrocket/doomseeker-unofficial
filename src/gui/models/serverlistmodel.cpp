//------------------------------------------------------------------------------
// serverlistmodel.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "gui/models/serverlistmodel.h"
#include "gui/widgets/serverlistview.h"
#include "main.h"
#include <QItemDelegate>
#include <QPainter>
#include <QTime>


ServerListColumn ServerListModel::columns[] =
{
	{ tr("Port"), 24, false, false },
	{ tr("Players"), 60, false, true },
	{ tr("Ping"), 50, false, true },
	{ tr("Servername"), 200, false, true },
	{ tr("Address"), 120, false, true },
	{ tr("IWAD"), 90, false, true },
	{ tr("MAP"), 70, false, true },
	{ tr("Wads"), 120, false, true },
	{ tr("Gametype"), 150, false, true },
	{ "SORT_GROUP", 0, true, false },
	{ "SERVER_POINTER", 0, true, false }
};

//////////////////////////////////////////////////////////////

class PlayersDiagram
{
	public:
		PlayersDiagram(const Server *server) : server(server), tmp(NULL)
		{
			if(openImage == NULL)
			{
				openImage = new QImage(":/slots/open");
				openSpecImage = new QImage(":/slots/specopen");
				botImage = new QImage(":/slots/bot");
				playerImage = new QImage(":/slots/player");
				spectatorImage = new QImage(":/slots/spectator");
			}
			QPixmap diagram(server->maximumClients()*playerImage->width(), playerImage->height());
			diagram.fill(Qt::transparent);

			int slotSize = playerImage->width();
			int position = diagram.width()-slotSize;
			QPainter p(&diagram);

			// numSpectators is actually greater than numPlayers.  This is done
			// in order to simplify the drawing code.
			int numSpectators = server->numPlayers();
			int numPlayers = numSpectators;
			int numBots = numSpectators; // Bots not on a team
			int numPlayersTeam[MAX_TEAMS] = {0, 0, 0, 0};
			int numBotsTeam[MAX_TEAMS] = {0, 0, 0, 0};
			for(unsigned int i = 0;i < server->numPlayers();i++)
			{
				if(server->player(i).isSpectating())
				{
					numPlayers--;
					numBots--;
				}
				else if(server->player(i).teamNum() != Player::TEAM_NONE)
				{
					if(!server->player(i).isBot())
						numPlayersTeam[server->player(i).teamNum()]++;
					numBotsTeam[server->player(i).teamNum()]++;
				}
				else if(server->player(i).isBot())
				{
					numPlayers--;
				}
			}

			// Draw them
			int currentTeam = 0;
			int count = 0;
			for(unsigned short i = 0;i < server->maximumClients();i++)
			{
				const QImage *slot = openSpecImage;
				if(i < numPlayers)
				{
					while(numBotsTeam[currentTeam] == count && currentTeam < MAX_TEAMS)
					{
						count = 0;
						currentTeam++;
					}
					if(currentTeam >= MAX_TEAMS)
						currentTeam = Player::TEAM_NONE;

					if(currentTeam == Player::TEAM_NONE || count < numPlayersTeam[currentTeam])
						slot = colorizePlayer(playerImage, QColor(server->teamColor(currentTeam)));
					else
						slot = colorizePlayer(botImage, QColor(server->teamColor(currentTeam)));
					count++;
				}
				else if(i < numBots)
					slot = colorizePlayer(botImage, QColor(server->teamColor(Player::TEAM_NONE)));
				else if(i < numSpectators)
					slot = spectatorImage;
				else if(i < server->maximumPlayers())
					slot = openImage;
				p.drawImage(position, 0, *slot);
				position -= slotSize;
			}

			this->diagram = diagram;
		}

		~PlayersDiagram()
		{
			if(tmp != NULL)
				delete tmp;
		}

		QPixmap pixmap() const { return diagram; }

	protected:
		static const QImage *openImage, *openSpecImage, *botImage, *playerImage, *spectatorImage;

		/**
		 * Colorizes the image to color.  This works is a fairly hacky way.  It
		 * colorizes cyan areas only.  To detect cyan it simply checks if a red
		 * component is present.  If so it is not cyan.
		 *
		 * Colorization is done by keeping the hue and saturation if the passed
		 * in color and applying the value of the color in the image.
		 */
		const QImage *colorizePlayer(const QImage *image, const QColor &color)
		{
			if(tmp != NULL)
				delete tmp;
			tmp = new QImage(*image);

			QVector<QRgb> colors = tmp->colorTable();
			QColor destinationColor = color.toHsv();
			for(int i = 0;i < colors.size();i++)
			{
				// Cyan has no red so move on if this color has red.
				if(qRed(colors[i]) != 0)
					continue;

				int hue = 0;
				int saturation = 0;
				int value = 0;
				destinationColor.getHsv(&hue, &saturation, &value);
				destinationColor.setHsv(hue, saturation, QColor(colors[i]).toHsv().value());
				colors[i] = destinationColor.rgb();
			}
			tmp->setColorTable(colors);

			return tmp;
		}

	private:
		const Server	*server;
		QPixmap			diagram;
		QImage			*tmp;
};

const QImage *PlayersDiagram::openImage = NULL;
const QImage *PlayersDiagram::openSpecImage = NULL;
const QImage *PlayersDiagram::botImage = NULL;
const QImage *PlayersDiagram::playerImage = NULL;
const QImage *PlayersDiagram::spectatorImage = NULL;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
ServerListModel::ServerListModel(QObject* parent) : QStandardItemModel(parent)
{
	setSortRole(SLDT_SORT);
}

int ServerListModel::addServer(Server* server, int response)
{
	QList<QStandardItem*> columns;

	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		columns.append(new QStandardItem());
	}

	appendRow(columns);
	QModelIndex index = indexFromItem(columns[0]);
	return updateServer(index.row(), server, response);
}

void ServerListModel::clearNonVitalFields(int row)
{
	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		if(columns[i].bHidden || i == SLCID_ADDRESS || i == SLCID_PORT)
		{
			continue;
		}
		emptyItem(item(row, i));
	}
}

void ServerListModel::destroyRows()
{
	int rows = rowCount();
	removeRows(0, rows);
	emit modelCleared();
}

void ServerListModel::emptyItem(QStandardItem* item)
{
	item->setData("", Qt::DisplayRole);
	item->setData(QVariant(), SLDT_SORT);
}

void ServerListModel::fillItem(QStandardItem* item, const QString& str)
{
	QString newStr = str.toLower();
	item->setData(str, Qt::DisplayRole);
	item->setData(newStr, SLDT_SORT);
}

void ServerListModel::fillItem(QStandardItem* item, int num)
{
	QVariant var = num;

	item->setData(var, Qt::DisplayRole);
	item->setData(var, SLDT_SORT);
}

void ServerListModel::fillItem(QStandardItem* item, const QHostAddress& addr, const QString& actualDisplay)
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
	item->setData(var, SLDT_SORT);
}

void ServerListModel::fillItem(QStandardItem* item, const QString& sort, const QPixmap& icon)
{
	item->setIcon(QIcon(icon));
	item->setData(sort, SLDT_SORT);
}

void ServerListModel::fillItem(QStandardItem* item, int sort, const QPixmap& image)
{
	item->setData(image, Qt::DecorationRole);
	item->setData(sort, SLDT_SORT);
}

QModelIndex ServerListModel::findServerOnTheList(const Server* server)
{
	if (server != NULL)
	{
		for (int i = 0; i < rowCount(); ++i)
		{
			QStandardItem* qstdItem = item(i, SLCID_HIDDEN_SERVER_POINTER);
			const Server* savedServ = serverFromList(qstdItem);
			if (server == savedServ)
			{
				QModelIndex index = indexFromItem(qstdItem);
				return index;
			}
		}
	}
	return QModelIndex();
}

void ServerListModel::removeCustomServers()
{
	QList<Server*> serversToRemove;
	for (int i = 0; i < rowCount(); ++i)
	{
		Server* serv = serverFromList(i);
		if (serv->isCustom())
			serversToRemove.append(serv);
	}

	QList<Server*>::iterator it;
	for (it = serversToRemove.begin(); it != serversToRemove.end(); ++it)
	{
		QModelIndex index = findServerOnTheList(*it);
		removeRow(index.row());
	}
}

void ServerListModel::setBackgroundColor(int row, Server* server)
{
	if (server->isCustom())
	{
		for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
		{
			QStandardItem* itm = item(row, i);
			itm->setBackground( QBrush(QColor(0x94, 0xff, 0xff)) );
		}
	}
}

void ServerListModel::setBad(int row, Server* server)
{
	QStandardItem* qstdItem;

	clearNonVitalFields(row);

	qstdItem = item(row, SLCID_SERVERNAME);
	fillItem(qstdItem, tr("<ERROR>"));

	qstdItem = item(row, SLCID_HIDDEN_GROUP);
	fillItem(qstdItem, SG_BAD);
}

void ServerListModel::setBanned(int row, Server* server)
{
	QStandardItem* qstdItem;

	clearNonVitalFields(row);

	qstdItem = item(row, SLCID_SERVERNAME);
	fillItem(qstdItem, tr("You are banned from this server!"));

	qstdItem = item(row, SLCID_HIDDEN_GROUP);
	fillItem(qstdItem, SG_BANNED);
}

void ServerListModel::setCountryFlag(QStandardItem* itm, const QHostAddress& addr)
{
	QPixmap flag = Main::ip2c->flag(addr);
	if (!flag.isNull())
	{
		itm->setIcon(flag);
	}
}

void ServerListModel::setGood(int row, Server* server)
{
	QStandardItem* qstdItem;
	QString strTmp;

	qstdItem = item(row, SLCID_PLAYERS);
	fillItem(qstdItem, server->numPlayers(), PlayersDiagram(server).pixmap());
	qstdItem->setData(USERROLE_RIGHTALIGNDECORATION, Qt::UserRole);

	qstdItem = item(row, SLCID_PING);
	fillItem(qstdItem, server->ping());

	qstdItem = item(row, SLCID_SERVERNAME);
	fillItem(qstdItem, server->name());

	qstdItem = item(row, SLCID_IWAD);
	fillItem(qstdItem, server->iwadName());

	qstdItem = item(row, SLCID_MAP);
	fillItem(qstdItem, server->map());

	strTmp = server->pwads().join(" ");
	qstdItem = item(row, SLCID_WADS);
	fillItem(qstdItem, strTmp);

	qstdItem = item(row, SLCID_GAMETYPE);
	fillItem(qstdItem, server->gameMode().name());

	qstdItem = item(row, SLCID_HIDDEN_GROUP);
	fillItem(qstdItem, SG_NORMAL);
}

void ServerListModel::setRefreshing(int row)
{
	QStandardItem* qstdItem = item(row, SLCID_SERVERNAME);
	qstdItem->setText(tr("<REFRESHING>"));
}

void ServerListModel::setTimeout(int row, Server* server)
{
	QStandardItem* qstdItem;

	clearNonVitalFields(row);

	qstdItem = item(row, SLCID_SERVERNAME);
	fillItem(qstdItem, tr("<NO RESPONSE>"));

	qstdItem = item(row, SLCID_HIDDEN_GROUP);
	fillItem(qstdItem, SG_TIMEOUT);
}

void ServerListModel::setWait(int row, Server* server)
{
	QStandardItem* qstdItem;

	clearNonVitalFields(row);

	qstdItem = item(row, SLCID_SERVERNAME);
	fillItem(qstdItem, tr("<Refreshed too soon, wait a while and try again>") );

	qstdItem = item(row, SLCID_HIDDEN_GROUP);
	fillItem(qstdItem, SG_WAIT);
}

Server* ServerListModel::serverFromList(int rowNum)
{
    QStandardItem* qstdItem = item(rowNum, SLCID_HIDDEN_SERVER_POINTER);
    return serverFromList(qstdItem);
}

Server* ServerListModel::serverFromList(const QModelIndex& index)
{
    QStandardItem* qstdItem = item(index.row(), SLCID_HIDDEN_SERVER_POINTER);
    return serverFromList(qstdItem);
}

Server* ServerListModel::serverFromList(const QStandardItem* item)
{
    QVariant pointer = qVariantFromValue(item->data(SLDT_POINTER_TO_SERVER_STRUCTURE));
    if (!pointer.isValid())
    {
        return NULL;
    }
    ServerPointer savedServ = qVariantValue<ServerPointer>(pointer);
    return savedServ.ptr;
}

ServerListModel::ServerGroup ServerListModel::serverGroup(int row)
{
	QStandardItem* qstdItem = item(row, SLCID_HIDDEN_GROUP);
	return static_cast<ServerListModel::ServerGroup>(qstdItem->data(SLDT_SORT).toInt());
}

void ServerListModel::prepareHeaders()
{
	QStringList labels;
	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		labels << columns[i].name;
	}
	setHorizontalHeaderLabels(labels);
}

int ServerListModel::updateServer(int row, Server* server, int response)
{
	QStandardItem* qstdItem;
	QStandardItem* itemPointer;

    itemPointer = item(row, SLCID_HIDDEN_SERVER_POINTER);
    // Save pointer to the column
	ServerPointer ptr(server);
	QVariant savePointer = qVariantFromValue(ptr);
	itemPointer->setData(savePointer, SLDT_POINTER_TO_SERVER_STRUCTURE);

	// Port icon is set no matter what
	qstdItem = item(row, SLCID_PORT);
	QPixmap icon = server->icon();
	if(server->isLocked() && server->isKnown()) // Draw a key if it is locked.
	{
		QPainter iconPainter(&icon);
		iconPainter.drawPixmap(0, 0, QPixmap(":/locked.png"));
		iconPainter.end();
	}
	fillItem(qstdItem, server->metaObject()->className(), icon);

	// Also the flag should be set no matter what
	qstdItem = item(row, SLCID_SERVERNAME);
	setCountryFlag(qstdItem, server->address());

	// Address is also set no matter what, so it's set here.
	qstdItem = item(row, SLCID_ADDRESS);
	fillItem(qstdItem, server->address(), QString(server->address().toString() + ":" + QString::number(server->port())) );

	switch(response)
	{
		case Server::RESPONSE_BAD:
			setBad(row, server);
			break;

		case Server::RESPONSE_BANNED:
			setBanned(row, server);
			break;

		case Server::RESPONSE_GOOD:
			setGood(row, server);
			break;

		case Server::RESPONSE_WAIT:
			if (server->isKnown())
			{
				setGood(row, server);
			}
			else
			{
				setWait(row, server);
			}
			break;

		case Server::RESPONSE_TIMEOUT:
		    setTimeout(row, server);
			break;
	}

	setBackgroundColor(row, server);

	return row;
}

QVariant ServerListModel::columnSortData(int row, int column)
{
	QStandardItem* it = item(row, column);
	return it->data(SLDT_SORT);
}

void ServerListModel::updateFlag(int row)
{
    Server* serv = serverFromList(row);
    QStandardItem* itm = item(row, SLCID_SERVERNAME);
    setCountryFlag(itm, serv->address());
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
bool ServerListSortFilterProxyModel::compareColumnSortData(QVariant& var1, QVariant& var2, int column) const
{
	if ( !(var1.isValid() && var2.isValid()) )
		return false;

	switch(column)
	{
		case ServerListModel::SLCID_ADDRESS:
			if (var1.toUInt() < var2.toUInt())
				return true;
			else
				return false;
			break;

		case ServerListModel::SLCID_PING:
		case ServerListModel::SLCID_PLAYERS:
			if (var1.toInt() < var2.toInt())
				return true;
			else
				return false;

			break;

		case ServerListModel::SLCID_PORT:
		case ServerListModel::SLCID_GAMETYPE:
		case ServerListModel::SLCID_IWAD:
		case ServerListModel::SLCID_MAP:
		case ServerListModel::SLCID_SERVERNAME:
		case ServerListModel::SLCID_WADS:
			if (var1.toString() < var2.toString())
				return true;
			else
				return false;
			break;
	}
}

bool ServerListSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	ServerListModel* model = static_cast<ServerListModel*>(sourceModel());

	Server* s1 = serverFromList(left);
	Server* s2 = serverFromList(right);

	if (s1 != NULL && s2 != NULL)
	{
		if (s1->isCustom() && !s2->isCustom())
		{
			return sortOrder == Qt::AscendingOrder;
		}
		else if (!s1->isCustom() && s2->isCustom())
		{
			return sortOrder == Qt::DescendingOrder;
		}
	}

	ServerListModel::ServerGroup sg1 = model->serverGroup(left.row());
	ServerListModel::ServerGroup sg2 = model->serverGroup(right.row());

	if (sg1 != sg2 && sg1 != 0 && sg2 != 0)
	{
		if (sg1 > sg2)
		{
			return sortOrder == Qt::AscendingOrder;
		}
		else
		{
			return sortOrder == Qt::DescendingOrder;
		}
	}
/*
	if (sg1 != sg2 && sg1 != 0 && sg2 != 0)
	{
		if (sortOrder == Qt::AscendingOrder)
		{
			if (sg1 > sg2)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (sg1 < sg2)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	*/

	QVariant leftVar = sourceModel()->data(left, sortRole());
	QVariant rightVar = sourceModel()->data(right, sortRole());

	return compareColumnSortData(leftVar, rightVar, left.column());
}

Server* ServerListSortFilterProxyModel::serverFromList(const QModelIndex& index) const
{
	ServerListModel* model = static_cast<ServerListModel*>(sourceModel());

    QStandardItem* qstdItem = model->item(index.row(), ServerListModel::SLCID_HIDDEN_SERVER_POINTER);
    return model->serverFromList(qstdItem);
}
