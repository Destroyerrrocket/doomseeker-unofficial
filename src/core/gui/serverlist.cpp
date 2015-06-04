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
#include "serverlist.h"

#include "configuration/doomseekerconfig.h"
#include "gui/remoteconsole.h"
#include "gui/models/serverlistcolumn.h"
#include "gui/models/serverlistmodel.h"
#include "gui/models/serverlistproxymodel.h"
#include "gui/widgets/serverlistcontextmenu.h"
#include "gui/widgets/serverlistview.h"
#include "refresher/refresher.h"
#include "serverapi/tooltips/servertooltip.h"
#include "serverapi/server.h"
#include "urlopener.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QToolTip>

using namespace ServerListColumnId;

ServerListHandler::ServerListHandler(ServerListView* serverTable, QWidget* pMainWindow)
: mainWindow(pMainWindow), model(NULL),
  sortingProxy(NULL), sortOrder(Qt::AscendingOrder),
  sortIndex(-1), table(serverTable)
{
	prepareServerTable();

	needsCleaning = false;

	initCleanerTimer();
}

ServerListHandler::~ServerListHandler()
{
	saveColumnsWidthsSettings();
}

void ServerListHandler::applyFilter(const ServerListFilterInfo& filterInfo)
{
	gConfig.serverFilter.info = filterInfo;
	sortingProxy->setFilterInfo(filterInfo);
	needsCleaning = true;
}

bool ServerListHandler::areColumnsWidthsSettingsChanged()
{
	for(int i = 0; i < NUM_SERVERLIST_COLUMNS; ++i)
	{
		if(ServerListColumns::columns[i].width != table->columnWidth(i))
		{
			return true;
		}
	}

	return false;
}

void ServerListHandler::clearTable()
{
	model->destroyRows();
}

void ServerListHandler::cleanUp()
{
	if (needsCleaning && mainWindow->isActiveWindow())
	{
		if (sortIndex >= 0)
		{
			ServerListProxyModel* pModel = static_cast<ServerListProxyModel*>(table->model());
			pModel->invalidate();
			pModel->sortServers(sortIndex, sortOrder);
		}

		setCountryFlagsIfNotPresent();
		table->updateAllRows();
		needsCleaning = false;
	}
}

void ServerListHandler::cleanUpForce()
{
	needsCleaning = true;
	cleanUp();
}

void ServerListHandler::clearAdditionalSorting()
{
	sortingProxy->clearAdditionalSorting();
}

void ServerListHandler::columnHeaderClicked(int index)
{
	if (isSortingByColumn(index))
	{
		sortOrder = swappedCurrentSortOrder();
	}
	else
	{
		sortOrder = getColumnDefaultSortOrder(index);
	}
	sortIndex = index;

	cleanUpForce();

	QHeaderView* header = table->horizontalHeader();
	header->setSortIndicator(sortIndex, sortOrder);
}

void ServerListHandler::connectTableModelProxySlots()
{
	QHeaderView* header = table->horizontalHeader();
	this->connect(header, SIGNAL(sectionClicked(int)), SLOT(columnHeaderClicked(int)));

	this->connect(model, SIGNAL(modelCleared()), SLOT(modelCleared()));

	this->connect(table->selectionModel(),
		SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
		SLOT(itemSelected(QItemSelection)));
	this->connect(table, SIGNAL(middleMouseClicked(QModelIndex, QPoint)),
		SLOT(tableMiddleClicked(QModelIndex, QPoint)));
	this->connect(table, SIGNAL(rightMouseClicked(QModelIndex, QPoint)),
		SLOT(tableRightClicked(QModelIndex, QPoint)));
	this->connect(table, SIGNAL(entered(QModelIndex)), SLOT(mouseEntered(QModelIndex)));
	this->connect(table, SIGNAL(leftMouseDoubleClicked(QModelIndex, QPoint)),
		SLOT(doubleClicked(QModelIndex)));
}

void ServerListHandler::contextMenuAboutToHide()
{
	sender()->deleteLater();
}

void ServerListHandler::contextMenuTriggered(QAction* action)
{
	ServerListContextMenu *contextMenu = static_cast<ServerListContextMenu*>(sender());
	ServerPtr server = contextMenu->server();
	// 1. This is a bit convoluted, but emitting the serverFilterModified
	//    signal leads to a call to applyFilter() in this class.
	// 2. Since the menu modifies existing server filter, the worst that can
	//    happen is that we set the same filter again.
	emit serverFilterModified(contextMenu->serverFilter());

	ServerListContextMenu::Result contextMenuResult = contextMenu->translateQMenuResult(action);
	switch (contextMenuResult)
	{
		case ServerListContextMenu::DataCopied:
			// Do nothing.
			break;

		case ServerListContextMenu::FindMissingWADs:
			emit findMissingWADs(server);
			break;

		case ServerListContextMenu::Join:
			emit serverDoubleClicked(server);
			break;

		case ServerListContextMenu::OpenRemoteConsole:
			new RemoteConsole(server);
			break;

		case ServerListContextMenu::OpenURL:
			// Calling QDesktopServices::openUrl() here directly resulted
			// in a crash somewhere in Qt libraries. UrlOpener defers the
			// call with a timer and this fixes the crash.
			UrlOpener::instance()->open(server->webSite());
			break;

		case ServerListContextMenu::NothingHappened:
			// Do nothing; ignore.
			break;

		case ServerListContextMenu::Refresh:
			refreshSelected();
			break;

		case ServerListContextMenu::ShowJoinCommandLine:
			emit displayServerJoinCommandLine(server);
			break;

		case ServerListContextMenu::SortAdditionallyAscending:
			sortAdditionally(contextMenu->modelIndex(), Qt::AscendingOrder);
			break;

		case ServerListContextMenu::SortAdditionallyDescending:
			sortAdditionally(contextMenu->modelIndex(), Qt::DescendingOrder);
			break;

		case ServerListContextMenu::RemoveAdditionalSortingForColumn:
			removeAdditionalSortingForColumn(contextMenu->modelIndex());
			break;

		case ServerListContextMenu::ClearAdditionalSorting:
			clearAdditionalSorting();
			break;

		default:
			QMessageBox::warning(mainWindow, tr("Doomseeker - context menu warning"),
				tr("Unhandled behavior in ServerListHandler::contextMenuTriggered()"));
			break;
	}
}

ServerListModel* ServerListHandler::createModel()
{
	ServerListModel* serverListModel = new ServerListModel(this);
	serverListModel->prepareHeaders();
	return serverListModel;
}

ServerListProxyModel *ServerListHandler::createSortingProxy(ServerListModel* serverListModel)
{
	ServerListProxyModel* proxy = new ServerListProxyModel(this);
	this->connect(proxy, SIGNAL(additionalSortColumnsChanged()),
		SLOT(updateHeaderTitles()));
	this->connect(proxy, SIGNAL(additionalSortColumnsChanged()),
		SLOT(saveAdditionalSortingConfig()));
	proxy->setSourceModel(serverListModel);
	proxy->setSortRole(ServerListModel::SLDT_SORT);
	proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	proxy->setFilterKeyColumn(IDServerName);

	return proxy;
}

void ServerListHandler::doubleClicked(const QModelIndex& index)
{
	emit serverDoubleClicked(serverFromIndex(index));
}

Qt::SortOrder ServerListHandler::getColumnDefaultSortOrder(int columnId)
{
	// Right now we can assume that columnIndex == columnId.
	return ServerListColumns::columns[columnId].defaultSortOrder;
}

bool ServerListHandler::hasAtLeastOneServer() const
{
	return model->rowCount() > 0;
}

void ServerListHandler::initCleanerTimer()
{
	cleanerTimer.setInterval(200);
	cleanerTimer.start();
	connect(&cleanerTimer, SIGNAL( timeout() ), this, SLOT ( cleanUp() ) );
}

bool ServerListHandler::isAnyColumnSortedAdditionally() const
{
	return sortingProxy->isAnyColumnSortedAdditionally();
}

bool ServerListHandler::isSortingAdditionallyByColumn(int column) const
{
	return sortingProxy->isSortingAdditionallyByColumn(column);
}

bool ServerListHandler::isSortingByColumn(int columnIndex)
{
	return sortIndex == columnIndex;
}

void ServerListHandler::itemSelected(const QItemSelection& selection)
{
	QSortFilterProxyModel* pModel = static_cast<QSortFilterProxyModel*>(table->model());
	QModelIndexList indexList = selection.indexes();

	QList<ServerPtr> servers;
	for(int i = 0; i < indexList.count(); ++i)
	{
		QModelIndex realIndex = pModel->mapToSource(indexList[i]);
		ServerPtr server = model->serverFromList(realIndex);
		servers.append(server);
	}
	emit serversSelected(servers);
}

void ServerListHandler::lookupHosts()
{
	for (int i = 0; i < model->rowCount(); ++i)
	{
		ServerPtr server = model->serverFromList(i);
		server->lookupHost();
	}
}

void ServerListHandler::modelCleared()
{
	QList<ServerPtr> servers;
	emit serversSelected(servers);
}

void ServerListHandler::mouseEntered(const QModelIndex& index)
{
	QSortFilterProxyModel* pModel = static_cast<QSortFilterProxyModel*>(table->model());
	QModelIndex realIndex = pModel->mapToSource(index);
	ServerPtr server = model->serverFromList(realIndex);
	QString tooltip;

	// Functions inside cases perform checks on the server structure
	// to see if any tooltip should be generated. Empty string is returned
	// in case if it should be not.
	switch(index.column())
	{
		case IDPort:
			tooltip = ServerTooltip::createPortToolTip(server);
			break;

		case IDAddress:
			tooltip = server->hostName(true);
			break;

		case IDPlayers:
			tooltip = ServerTooltip::createPlayersToolTip(server);
			break;

		case IDServerName:
			tooltip = ServerTooltip::createServerNameToolTip(server);
			break;

		case IDIwad:
			tooltip = ServerTooltip::createIwadToolTip(server);
			break;

		case IDWads:
			tooltip = ServerTooltip::createPwadsToolTip(server);
			break;

		default:
			tooltip = "";
			break;
	}

	QToolTip::showText(QCursor::pos(), tooltip, NULL);
}

void ServerListHandler::prepareServerTable()
{
	model = createModel();
	sortingProxy = createSortingProxy(model);

	columnHeaderClicked(IDPlayers);
	table->setModel(sortingProxy);
	table->setupTableProperties();

	if(gConfig.doomseeker.serverListSortIndex >= 0)
	{
		sortIndex = gConfig.doomseeker.serverListSortIndex;
		sortOrder = static_cast<Qt::SortOrder> (gConfig.doomseeker.serverListSortDirection);
	}

	connectTableModelProxySlots();
	sortingProxy->setAdditionalSortColumns(gConfig.doomseeker.additionalSortColumns());
}

void ServerListHandler::redraw()
{
	model->redrawAll();
}

void ServerListHandler::refreshAll()
{
	for (int i = 0; i < model->rowCount(); ++i)
	{
		gRefresher->registerServer(model->serverFromList(i).data());
	}
}

void ServerListHandler::refreshSelected()
{
	QItemSelectionModel* selectionModel = table->selectionModel();
	QModelIndexList indexList = selectionModel->selectedRows();

	for(int i = 0; i < indexList.count(); ++i)
	{
		QModelIndex realIndex = sortingProxy->mapToSource(indexList[i]);
		gRefresher->registerServer(model->serverFromList(realIndex).data());
	}
}

void ServerListHandler::removeAdditionalSortingForColumn(const QModelIndex &modelIndex)
{
	sortingProxy->removeAdditionalColumnSorting(modelIndex.column());
}

void ServerListHandler::removeServer(const ServerPtr &server)
{
	model->removeServer(server);
}

void ServerListHandler::saveAdditionalSortingConfig()
{
	gConfig.doomseeker.setAdditionalSortColumns(sortingProxy->additionalSortColumns());
}

void ServerListHandler::saveColumnsWidthsSettings()
{
	gConfig.doomseeker.serverListColumnState = table->horizontalHeader()->saveState().toBase64();
	gConfig.doomseeker.serverListSortIndex = sortIndex;
	gConfig.doomseeker.serverListSortDirection = sortOrder;
}

QList<ServerPtr> ServerListHandler::selectedServers()
{
	QModelIndexList indexList = table->selectionModel()->selectedRows();

	QList<ServerPtr> servers;
	for(int i = 0; i < indexList.count(); ++i)
	{
		QModelIndex realIndex = sortingProxy->mapToSource(indexList[i]);
		ServerPtr server = model->serverFromList(realIndex);
		servers.append(server);
	}
	return servers;
}

void ServerListHandler::serverBegunRefreshing(const ServerPtr &server)
{
	model->setRefreshing(server);
}

ServerPtr ServerListHandler::serverFromIndex(const QModelIndex &index)
{
	QSortFilterProxyModel* pModel = static_cast<QSortFilterProxyModel*>(table->model());
	QModelIndex indexReal = pModel->mapToSource(index);
	return model->serverFromList(indexReal);
}

void ServerListHandler::serverUpdated(const ServerPtr &server, int response)
{
	int rowIndex = model->findServerOnTheList(server.data());
	if (rowIndex >= 0)
	{
		rowIndex = model->updateServer(rowIndex, server, response);
	}
	else
	{
		rowIndex = model->addServer(server, response);
	}

	needsCleaning = true;
	emit serverInfoUpdated(server);
}

void ServerListHandler::setCountryFlagsIfNotPresent()
{
	const bool FORCE = true;
	updateCountryFlags(!FORCE);
}

void ServerListHandler::setGroupServersWithPlayersAtTop(bool b)
{
	sortingProxy->setGroupServersWithPlayersAtTop(b);
}

void ServerListHandler::sortAdditionally(const QModelIndex &modelIndex, Qt::SortOrder order)
{
	ServerListProxyModel* model = static_cast<ServerListProxyModel*>(table->model());
	model->addAdditionalColumnSorting(modelIndex.column(), order);
}

Qt::SortOrder ServerListHandler::swappedCurrentSortOrder()
{
	return sortOrder == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
}

void ServerListHandler::tableMiddleClicked(const QModelIndex& index, const QPoint& cursorPosition)
{
	refreshSelected();
}

void ServerListHandler::tableRightClicked(const QModelIndex& index, const QPoint& cursorPosition)
{
	ServerPtr server = serverFromIndex(index);

	ServerListContextMenu *contextMenu = new ServerListContextMenu(server,
		sortingProxy->filterInfo(), index, this);
	this->connect(contextMenu, SIGNAL(aboutToHide()), SLOT(contextMenuAboutToHide()));
	this->connect(contextMenu, SIGNAL(triggered(QAction*)), SLOT(contextMenuTriggered(QAction*)));

	QPoint displayPoint = table->viewport()->mapToGlobal(cursorPosition);
	contextMenu->popup(displayPoint);
}

void ServerListHandler::updateCountryFlags()
{
	const bool FORCE = true;
	updateCountryFlags(FORCE);
}

void ServerListHandler::updateCountryFlags(bool force)
{
	for (int i = 0; i < model->rowCount(); ++i)
	{
		model->updateFlag(i, force);
	}
}

void ServerListHandler::updateHeaderTitles()
{
	const QList<ColumnSort> &sortings = sortingProxy->additionalSortColumns();
	for (int i = 0; i < ServerListColumnId::NUM_SERVERLIST_COLUMNS; ++i)
	{
		// Clear header icons.
		model->setHeaderData(i, Qt::Horizontal, QIcon(), Qt::DecorationRole);
	}
	QStringList labels = ServerListColumns::generateColumnHeaderLabels();
	for (int i = 0; i < sortings.size(); ++i)
	{
		const ColumnSort &sort = sortings[i];
		labels[sort.columnId()] = QString("[%1] %2").arg(i + 1).arg(labels[sort.columnId()]);
		QIcon icon = sort.order() == Qt::AscendingOrder ?
			QIcon(":/icons/ascending.png") :
			QIcon(":/icons/descending.png");
		model->setHeaderData(sort.columnId(), Qt::Horizontal, icon, Qt::DecorationRole);
	}
	model->setHorizontalHeaderLabels(labels);
}

void ServerListHandler::updateSearch(const QString& search)
{
	QRegExp pattern(QString("*") + search + "*", Qt::CaseInsensitive, QRegExp::Wildcard);
	sortingProxy->setFilterRegExp(pattern);
}

void ServerListHandler::registerServer(ServerPtr server)
{
	this->connect(server.data(), SIGNAL(updated(ServerPtr, int)),
		SLOT(serverUpdated(ServerPtr, int)));
	this->connect(server.data(), SIGNAL(begunRefreshing(ServerPtr)),
		SLOT(serverBegunRefreshing(ServerPtr)));
}

void ServerListHandler::deregisterServer(const ServerPtr &server)
{
	server->disconnect(this);
}
