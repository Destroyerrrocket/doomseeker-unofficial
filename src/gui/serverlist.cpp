//------------------------------------------------------------------------------
// serverlist.cpp
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

#include "gui/serverlist.h"
#include <QHeaderView>
#include <QStandardItem>
#include <QToolTip>


SLHandler::SLHandler(ServerListView* tab)
{
	table = tab;
	prepareServerTable();
	needsCleaning = false;
	cleaner.setInterval(200);
	cleaner.start();
	connect(&cleaner, SIGNAL( timeout() ), this, SLOT ( cleanUp() ) );
}

SLHandler::~SLHandler()
{
}

void SLHandler::clearTable()
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	model->destroyRows();
}

void SLHandler::cleanUp()
{
	if (needsCleaning)
	{
		ServerListModel* model = static_cast<ServerListModel*>(table->model());
		if (sortIndex >= 0)
		{
			model->sort(sortIndex, sortOrder);
		}

		table->updateAllRows();
		needsCleaning = false;
	}
}

void SLHandler::columnHeaderClicked(int index)
{
	// if user clicked on different column than the sorting is currently set on
	if (sortIndex != index)
	{
		// set sorting order to default for current column
		switch(index)
		{
			case ServerListModel::SLCID_PLAYERS:
				sortOrder = Qt::DescendingOrder;
				break;

			default:
				sortOrder = Qt::AscendingOrder;
				break;
		}
	}
	// if user clicked on the same column
	else
	{
		// change sorting order
		if (sortOrder == Qt::AscendingOrder)
		{
			sortOrder = Qt::DescendingOrder;
		}
		else
		{
			sortOrder = Qt::AscendingOrder;
		}
	}
	sortIndex = index;

	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	model->sort(sortIndex, sortOrder);

	QHeaderView* header = table->horizontalHeader();
	header->setSortIndicator(sortIndex, sortOrder);

	//table->fixRowSize();
}

QString SLHandler::createPlayersToolTip(const Server* server)
{
	if (server == NULL)
		return QString();

    QString ret;
	ret = "<div style='white-space: pre'>";
	ret += server->gameInfoTableHTML();
	ret += server->playerTableHTML();
	ret += "</div>";
	return ret;
}

QString SLHandler::createServerNameToolTip(const Server* server)
{
	if (server == NULL)
		return QString();

	QString ret;
	ret = "<div style='white-space: pre'>";
	ret += server->generalInfoHTML();
	ret += "</div>";
	return ret;
}

QString SLHandler::createPwadsToolTip(const Server* server)
{
	if (server == NULL)
		return QString();

	QString ret;
	ret = "<div style='white-space: pre'>";
	ret += server->pwads().join("\n");
	ret += "</div>";
	return ret;
}

void SLHandler::doubleClicked(const QModelIndex& index)
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	Server* server = model->serverFromList(index);
	emit serverDoubleClicked(server);
}

void SLHandler::itemSelected(const QModelIndex& index)
{
	QList<Server*> servers = selectedServers();
	emit serversSelected(servers);
}

void SLHandler::modelCleared()
{
	QList<Server*> servers;
	emit serversSelected(servers);
}

void SLHandler::mouseEntered(const QModelIndex& index)
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	Server* server = model->serverFromList(index);
	QString tooltip;

	if (!server->isKnown())
	{
		tooltip = "";
	}
	else
	{
		switch(index.column())
		{
			case ServerListModel::SLCID_PLAYERS:
				tooltip = createPlayersToolTip(server);
				break;

			case ServerListModel::SLCID_SERVERNAME:
				tooltip = createServerNameToolTip(server);
				break;

			case ServerListModel::SLCID_WADS:
				if (server->numWads() == 0)
				{
					tooltip = "";
				}
				else
				{
					tooltip = createPwadsToolTip(server);
				}
				break;

			default:
				tooltip = "";
				break;
		}
	}

	QToolTip::showText(QCursor::pos(), tooltip, table);
}

QList<Server*> SLHandler::selectedServers()
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	QItemSelectionModel* selModel = table->selectionModel();
	QModelIndexList indexList = selModel->selectedRows();

	QList<Server*> servers;
	for(int i = 0; i < indexList.count(); ++i)
	{
		Server* server = model->serverFromList(indexList[i]);
		servers.append(server);
	}
	return servers;
}

void SLHandler::prepareServerTable()
{
	sortOrder = Qt::AscendingOrder;
	sortIndex = -1;

	ServerListModel* model = new ServerListModel(this);
	model->prepareHeaders();
	table->setModel(model);

	// Now set column widths and other properties
	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		ServerListColumn* columns = ServerListModel::columns;
		table->setColumnWidth(i, columns[i].width);
		table->setColumnHidden(i, columns[i].bHidden);
		if(!columns[i].bResizable)
			table->horizontalHeader()->setResizeMode(i, QHeaderView::Fixed);
	}

	// We don't really need a vertical header so lets remove it
	table->verticalHeader()->hide();
	// Also some other flags that can't be set from the Designer
	table->horizontalHeader()->setSortIndicatorShown(true);
	table->horizontalHeader()->setHighlightSections(false);

	table->setMouseTracking(true);

	QHeaderView* header = table->horizontalHeader();
	connect(header, SIGNAL( sectionClicked(int) ), this, SLOT ( columnHeaderClicked(int) ) );
	connect(model, SIGNAL( modelCleared() ), this, SLOT( modelCleared() ) );
	connect(table, SIGNAL( clicked(const QModelIndex&) ), this, SLOT( itemSelected(const QModelIndex&) ));
	connect(table, SIGNAL( rightMouseClick(const QModelIndex&) ), this, SLOT ( itemSelected(const QModelIndex&)) );
	connect(table, SIGNAL( rightMouseClick(const QModelIndex&) ), this, SLOT ( tableRightClicked(const QModelIndex&)) );
	connect(table, SIGNAL( entered(const QModelIndex&) ), this, SLOT ( mouseEntered(const QModelIndex&)) );
	connect(table, SIGNAL( leftMouseDoubleClicked(const QModelIndex&)), this, SLOT( doubleClicked(const QModelIndex&)) );

	columnHeaderClicked(0);
}

void SLHandler::refreshAll()
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	for (int i = 0; i < table->model()->rowCount(); ++i)
	{
		model->setRefreshing(i);
	}
}

void SLHandler::serverUpdated(Server *server, int response)
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	QModelIndex index = model->findServerOnTheList(server);
	int row = 0;
	if (index.isValid())
	{
		row = model->updateServer(index.row(), server, response);
	}
	else
	{
		row = model->addServer(server, response);
	}

	needsCleaning = true;
}


void SLHandler::tableRightClicked(const QModelIndex& index)
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	QItemSelectionModel* selModel = table->selectionModel();
	QModelIndexList indexList = selModel->selectedRows();

	for(int i = 0; i < indexList.count(); ++i)
	{
		model->setRefreshing(indexList[i].row());
	}
}
