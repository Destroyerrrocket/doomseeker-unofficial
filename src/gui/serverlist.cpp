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

#include "gui/remoteconsole.h"
#include "gui/serverlist.h"
#include "main.h"
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QHeaderView>
#include <QMenu>
#include <QStandardItem>
#include <QToolTip>


SLHandler::SLHandler(ServerListView* tab)
{
	// We will be saving the column widths to the config, but lets get the defaults
	ServerListColumn* columns = ServerListModel::columns;
	QString widths;
	for(int i = 0;i < ServerListModel::HOW_MANY_SERVERLIST_COLUMNS;i++)
	{
		if(i != 0)
			widths += ",";
		widths += QString("%1").arg(columns[i].width);
	}
	Main::config->createSetting("ServerListColumnWidths", widths.toAscii().data());
	// Now we see if we have a different configuration.
	QStringList colWidths = Main::config->setting("ServerListColumnWidths")->string().split(',', QString::SkipEmptyParts);
	if(colWidths.size() == ServerListModel::HOW_MANY_SERVERLIST_COLUMNS) // If the number of columns do not match than reset this setting
	{
		for(int i = 0;i < ServerListModel::HOW_MANY_SERVERLIST_COLUMNS;i++)
		{
			bool ok = false;
			int width = colWidths[i].toInt(&ok);
			if(ok)
				columns[i].width = width;
		}
	}

	table = tab;
	table->setIconSize(QSize(26, 15));
	prepareServerTable();
	needsCleaning = false;
	cleaner.setInterval(200);
	cleaner.start();
	connect(&cleaner, SIGNAL( timeout() ), this, SLOT ( cleanUp() ) );

	// Finally, make all flags update with ip2c database
	connect(Main::ip2c, SIGNAL( databaseUpdated() ), this, SLOT( updateCountryFlags() ));
}

SLHandler::~SLHandler()
{
	// Have we made any changes to the column widths?
	for(int i = 0;i < ServerListModel::HOW_MANY_SERVERLIST_COLUMNS;i++)
	{
		if(ServerListModel::columns[i].width != table->columnWidth(i))
		{ // They have, write the changes.
			QString widths;
			for(int j = 0;j < ServerListModel::HOW_MANY_SERVERLIST_COLUMNS;j++)
			{
				if(j != 0)
					widths += ",";
				widths += QString("%1").arg(table->columnWidth(j));
			}
			Main::config->setting("ServerListColumnWidths")->setValue(widths);
			break;
		}
	}
}

void SLHandler::clearTable()
{
	model->destroyRows();
}

void SLHandler::cleanUp()
{
	if (needsCleaning && Main::mainWindow->isActiveWindow())
	{
		if (sortIndex >= 0)
		{
			ServerListSortFilterProxyModel* pModel = static_cast<ServerListSortFilterProxyModel*>(table->model());
			pModel->invalidate();
			pModel->sortServers(sortIndex, sortOrder);
		}

		updateCountryFlags(true);
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

	needsCleaning = true;
	cleanUp();

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
	if(server->numPlayers() != 0)
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
	emit serverDoubleClicked(serverFromIndex(index));
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
	QSortFilterProxyModel* pModel = static_cast<QSortFilterProxyModel*>(table->model());
	QModelIndex realIndex = pModel->mapToSource(index);
	Server* server = model->serverFromList(realIndex);
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

void SLHandler::prepareServerTable()
{
	sortOrder = Qt::AscendingOrder;
	sortIndex = -1;

	model = new ServerListModel(this);
	model->prepareHeaders();

	sortingModel = new ServerListSortFilterProxyModel(this);
	sortingModel->setSourceModel(model);
	sortingModel->setSortRole(ServerListModel::SLDT_SORT);
	sortingModel->setSortCaseSensitivity( Qt::CaseInsensitive );
	sortingModel->setFilterKeyColumn(ServerListModel::SLCID_SERVERNAME);

	table->setModel(sortingModel);

	// Now set column widths and other properties
	for (int i = 0; i < ServerListModel::HOW_MANY_SERVERLIST_COLUMNS; ++i)
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
	connect(sortingModel, SIGNAL( rowsInserted(const QModelIndex&, int, int) ), this, SLOT( resizeChangedRows(const QModelIndex&, int, int) ));
	connect(model, SIGNAL( modelCleared() ), this, SLOT( modelCleared() ) );
	connect(table, SIGNAL( clicked(const QModelIndex&) ), this, SLOT( itemSelected(const QModelIndex&) ));
	connect(table, SIGNAL( rightMouseClick(const QModelIndex&, const QPoint&) ), this, SLOT ( itemSelected(const QModelIndex&)) );
	connect(table, SIGNAL( rightMouseClick(const QModelIndex&, const QPoint&) ), this, SLOT ( tableRightClicked(const QModelIndex&, const QPoint&)) );
	connect(table, SIGNAL( entered(const QModelIndex&) ), this, SLOT ( mouseEntered(const QModelIndex&)) );
	connect(table, SIGNAL( leftMouseDoubleClicked(const QModelIndex&, const QPoint&)), this, SLOT( doubleClicked(const QModelIndex&)) );

	columnHeaderClicked(ServerListModel::SLCID_PLAYERS);
}

void SLHandler::redraw()
{
	model->redrawAll();
}

void SLHandler::refreshAll()
{
	for (int i = 0; i < model->rowCount(); ++i)
	{
		Server* serv = model->serverFromList(i);
		serv->refresh();
	}
}

void SLHandler::resizeChangedRows(const QModelIndex &parent, int start, int end)
{
	// This is so when the search is undone the rows don't become fat again.
	for (int i = start;i < end; ++i)
	{
		table->resizeRowToContents(i);
	}
}

QList<Server*> SLHandler::selectedServers()
{
	QSortFilterProxyModel* pModel = static_cast<QSortFilterProxyModel*>(table->model());
	QItemSelectionModel* selModel = table->selectionModel();
	QModelIndexList indexList = selModel->selectedRows();

	QList<Server*> servers;
	for(int i = 0; i < indexList.count(); ++i)
	{
		QModelIndex realIndex = pModel->mapToSource(indexList[i]);
		Server* server = model->serverFromList(realIndex);
		servers.append(server);
	}
	return servers;
}

void SLHandler::serverBegunRefreshing(Server* server)
{
	QModelIndex index = model->findServerOnTheList(server);
	if (index.isValid())
	{
		model->setRefreshing(index.row());
	}
}

Server *SLHandler::serverFromIndex(const QModelIndex &index)
{
	QSortFilterProxyModel* pModel = static_cast<QSortFilterProxyModel*>(table->model());
	QModelIndex indexReal = pModel->mapToSource(index);
	return model->serverFromList(indexReal);
}

void SLHandler::serverUpdated(Server *server, int response)
{
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

	table->resizeRowToContents(row);

	needsCleaning = true;
}


void SLHandler::tableRightClicked(const QModelIndex& index, const QPoint& point)
{
	Server *server = serverFromIndex(index);

	QAction* copyEmail = NULL;
	QAction* copyUrl = NULL;
	QAction* openUrlInDefaultBrowser = NULL;

	QMenu contextMenu;
	QAction *refresh = contextMenu.addAction(tr("Refresh"));
	QAction *join = contextMenu.addAction(tr("Join"));
	QAction* joinCommandLine = contextMenu.addAction(tr("Show join command line"));

	if (!server->website().isEmpty() && !server->website().startsWith("file://", Qt::CaseInsensitive))
	{
		openUrlInDefaultBrowser = contextMenu.addAction(tr("Open URL in browser"));
	}

	QMenu *copyMenu = contextMenu.addMenu(tr("Copy"));
	QAction *copyAddress = copyMenu->addAction(tr("Copy Address"));

	if (!server->eMail().isEmpty())
	{
		copyEmail = copyMenu->addAction(tr("Copy E-Mail"));
	}

	if (!server->website().isEmpty())
	{
		copyUrl = copyMenu->addAction(tr("Copy URL"));
	}

	QAction *copyName = copyMenu->addAction(tr("Copy Name"));
	QAction *rcon = NULL;
	if(server->hasRcon())
	{
		contextMenu.addSeparator();
		rcon = contextMenu.addAction("Remote Console");
	}

	QAction *result = contextMenu.exec(table->viewport()->mapToGlobal(point));
	if(result == refresh)
	{
		QSortFilterProxyModel* pModel = static_cast<QSortFilterProxyModel*>(table->model());
		QItemSelectionModel* selModel = table->selectionModel();
		QModelIndexList indexList = selModel->selectedRows();

		for(int i = 0; i < indexList.count(); ++i)
		{
			QModelIndex realIndex = pModel->mapToSource(indexList[i]);
			Server* serv = model->serverFromList(realIndex);
			serv->refresh();
		}
	}
	else if(result == join)
	{
		emit serverDoubleClicked(server);
	}
	else if (result == joinCommandLine)
	{
		emit displayServerJoinCommandLine(server);
	}
	else if (result == openUrlInDefaultBrowser && openUrlInDefaultBrowser != NULL)
	{
		QDesktopServices::openUrl(server->website());
	}
	else if(result == copyAddress)
	{
		QString addr = QString("%1:%2").arg(server->address().toString()).arg(server->port());
		QApplication::clipboard()->setText(addr);
	}
	else if (result == copyEmail && copyEmail != NULL)
	{
		QApplication::clipboard()->setText(server->eMail());
	}
	else if(result == copyName)
	{
		QApplication::clipboard()->setText(server->name());
	}
	else if (result == copyUrl && copyUrl != NULL)
	{
		QApplication::clipboard()->setText(server->website());
	}
	else if(result == rcon && rcon != NULL)
	{
		new RemoteConsole(server);
	}
}

void SLHandler::updateCountryFlags(bool onlyIfServerHasNoFlagYet)
{
    for (int i = 0; i < model->rowCount(); ++i)
    {
        model->updateFlag(i, onlyIfServerHasNoFlagYet);
    }
}

void SLHandler::updateSearch(const QString& search)
{
	QRegExp pattern(QString("*") + search + "*", Qt::CaseInsensitive, QRegExp::Wildcard);
	sortingModel->setFilterRegExp(pattern);
}
