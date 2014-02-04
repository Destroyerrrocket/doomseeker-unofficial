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
#include "configuration/doomseekerconfig.h"
#include "gui/remoteconsole.h"
#include "gui/serverlist.h"
#include "gui/models/serverlistcolumn.h"
#include "gui/models/serverlistproxymodel.h"
#include "gui/models/serverlistrowhandler.h"
#include "gui/widgets/serverlistcontextmenu.h"
#include "pathfinder/pathfinder.h"
#include "plugins/engineplugin.h"
#include "serverapi/gameexeretriever.h"
#include "serverapi/message.h"
#include "serverapi/playerslist.h"
#include "serverapi/server.h"
#include "serverapi/serverstructs.h"
#include "serverapi/tooltipgenerator.h"
#include "main.h"
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QToolTip>

const QString ServerListHandler::FONT_COLOR_MISSING = "#ff0000";
const QString ServerListHandler::FONT_COLOR_OPTIONAL = "#ff9f00";
const QString ServerListHandler::FONT_COLOR_FOUND = "#009f00";

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

	ServerListProxyModel* pModel = static_cast<ServerListProxyModel*>(table->model());
	pModel->setFilterInfo(filterInfo);

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

void ServerListHandler::columnHeaderClicked(int index)
{
	if (isSortingByColumn(index))
	{
		sortOrder = swapCurrentSortOrder();
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
	connect(header, SIGNAL( sectionClicked(int) ), this, SLOT ( columnHeaderClicked(int) ) );
	connect(model, SIGNAL( modelCleared() ), this, SLOT( modelCleared() ) );
	connect(table, SIGNAL( clicked(const QModelIndex&) ), this, SLOT( itemSelected(const QModelIndex&) ));
	connect(table, SIGNAL( middleMouseClick(const QModelIndex&, const QPoint&) ), this, SLOT( tableMiddleClicked(const QModelIndex&, const QPoint&) ) );
	connect(table, SIGNAL( rightMouseClick(const QModelIndex&, const QPoint&) ), this, SLOT ( itemSelected(const QModelIndex&)) );
	connect(table, SIGNAL( rightMouseClick(const QModelIndex&, const QPoint&) ), this, SLOT ( tableRightClicked(const QModelIndex&, const QPoint&)) );
	connect(table, SIGNAL( entered(const QModelIndex&) ), this, SLOT ( mouseEntered(const QModelIndex&)) );
	connect(table, SIGNAL( leftMouseDoubleClicked(const QModelIndex&, const QPoint&)), this, SLOT( doubleClicked(const QModelIndex&)) );
}

QString ServerListHandler::createIwadToolTip(const Server* server)
{
	if (!server->isKnown())
	{
		return QString();
	}

	// This will only return anything if we have the "TellMe..." option enabled.
	bool bFindIwad = gConfig.doomseeker.bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn;

	if (bFindIwad)
	{
		static const QString FORMAT_TEMPLATE = "<font color=\"%1\">%2</font>";

		Message binMessage;
		// Use offline binary so that testing builds are not triggered.
		QString binPath = GameExeRetriever(*server->plugin()->gameExe()).pathToOfflineExe(binMessage);

		PathFinder pathFinder;
		pathFinder.addPrioritySearchDir(binPath);
		QString path = pathFinder.findFile(server->iwad());

		if (path.isEmpty())
		{
			return FORMAT_TEMPLATE.arg(FONT_COLOR_MISSING, tr("MISSING"));
		}
		else
		{
			return FORMAT_TEMPLATE.arg(FONT_COLOR_FOUND, path);
		}
	}

	return QString();
}

ServerListModel* ServerListHandler::createModel()
{
	ServerListModel* serverListModel = new ServerListModel(this);
	serverListModel->prepareHeaders();

	return serverListModel;
}

QString ServerListHandler::createPlayersToolTip(const Server* server)
{
	if (server == NULL || !server->isKnown())
	{
		return QString();
	}

	TooltipGenerator* tooltipGenerator = server->tooltipGenerator();

	QString ret;
	ret = "<div style='white-space: pre'>";
	ret += tooltipGenerator->gameInfoTableHTML();
	if(server->players()->numClients() != 0)
	{
		ret += tooltipGenerator->playerTableHTML();
	}
	ret += "</div>";

	delete tooltipGenerator;
	return ret;
}

QString ServerListHandler::createPortToolTip(const Server* server)
{
	if (server == NULL || !server->isKnown())
		return QString();

	QString ret;
	if (server->isLocked())
		ret = "Password protected";
	if (server->isSecure())
	{
		if(!ret.isEmpty())
			ret += '\n';
		ret += "Enforces master bans";
	}
	return ret;
}

QString ServerListHandler::createPwadsToolTip(const Server* server)
{
	if (server == NULL || !server->isKnown() || server->numWads() == 0)
	{
		return QString();
	}

	// Prepare initial formatting.
	static const QString toolTip = "<div style='white-space: pre'>%1</div>";
	QString content;

	const QList<PWad>& pwads = server->wads();

	// Check if we should seek and colorize.
	bool bFindWads = gConfig.doomseeker.bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn;

	// Engage!
	if (bFindWads)
	{
		Message binMessage;
		// Use offline binary so that testing builds are not triggered.
		QString binPath = GameExeRetriever(*server->plugin()->gameExe()).pathToOfflineExe(binMessage);

		QStringList pwadsFormatted;
		foreach (const PWad &wad, pwads)
		{
			pwadsFormatted << createPwadToolTipInfo(wad, binPath);
		}

		content = "<table cellspacing=1>";
		content += pwadsFormatted.join("\n");
		content += "</table>";
	}
	else
	{
		foreach (const PWad &wad, pwads)
		{
			content += wad.name() + "\n";
		}
		content.chop(1); // Get rid of extra \n.
	}

	return toolTip.arg(content);
}

QString ServerListHandler::createPwadToolTipInfo(const PWad& pwad, const QString &binPath)
{
	QString formattedStringBegin = "<tr style=\"color: %1;\">";
	QString formattedStringEnd = "</tr>";
	QString formattedStringMiddle;

	PathFinder pathFinder;
	pathFinder.addPrioritySearchDir(binPath);
	QString pathToFile = pathFinder.findFile(pwad.name());

	if (pathToFile.isEmpty())
	{
		if(pwad.isOptional())
		{
			formattedStringBegin = formattedStringBegin.arg(FONT_COLOR_OPTIONAL);
			formattedStringMiddle = tr("<td>%1</td><td> OPTIONAL</td>").arg(pwad.name());
		}
		else
		{
			formattedStringBegin = formattedStringBegin.arg(FONT_COLOR_MISSING);
			formattedStringMiddle = tr("<td>%1</td><td> MISSING</td>").arg(pwad.name());
		}
	}
	else
	{
		formattedStringBegin = formattedStringBegin.arg(FONT_COLOR_FOUND);
		formattedStringMiddle = QString("<td>%1</td><td> %2</td>").arg(pwad.name(), pathToFile);
	}

	return formattedStringBegin + formattedStringMiddle + formattedStringEnd;
}

QString ServerListHandler::createServerNameToolTip(const Server* server)
{
	if (server == NULL)
	{
		return QString();
	}

	TooltipGenerator* tooltipGenerator = server->tooltipGenerator();

	QString ret;
	QString generalInfo = tooltipGenerator->generalInfoHTML();

	if (!generalInfo.isEmpty())
	{
		ret = "<div style='white-space: pre'>";
		ret += generalInfo;
		ret += "</div>";
	}

	delete tooltipGenerator;
	return ret;
}

QSortFilterProxyModel* ServerListHandler::createSortingProxy(ServerListModel* serverListModel)
{
	ServerListProxyModel* proxy = new ServerListProxyModel(this);
	proxy->setSourceModel(serverListModel);
	proxy->setSortRole(ServerListModel::SLDT_SORT);
	proxy->setSortCaseSensitivity( Qt::CaseInsensitive );
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

bool ServerListHandler::isSortingByColumn(int columnIndex)
{
	return sortIndex == columnIndex;
}

void ServerListHandler::itemSelected(const QModelIndex& index)
{
	QList<Server*> servers = selectedServers();
	emit serversSelected(servers);
}

void ServerListHandler::lookupHosts()
{
	for (int i = 0; i < model->rowCount(); ++i)
	{
		Server* server = model->serverFromList(i);
		server->lookupHost();
	}
}

void ServerListHandler::modelCleared()
{
	QList<Server*> servers;
	emit serversSelected(servers);
}

void ServerListHandler::mouseEntered(const QModelIndex& index)
{
	QSortFilterProxyModel* pModel = static_cast<QSortFilterProxyModel*>(table->model());
	QModelIndex realIndex = pModel->mapToSource(index);
	Server* server = model->serverFromList(realIndex);
	QString tooltip;

	// Functions inside cases perform checks on the server structure
	// to see if any tooltip should be generated. Empty string is returned
	// in case if it should be not.
	switch(index.column())
	{
		case IDPort:
			tooltip = createPortToolTip(server);
			break;

		case IDAddress:
			tooltip = server->hostName(true);
			break;

		case IDPlayers:
			tooltip = createPlayersToolTip(server);
			break;

		case IDServerName:
			tooltip = createServerNameToolTip(server);
			break;

		case IDIwad:
			tooltip = createIwadToolTip(server);
			break;

		case IDWads:
			tooltip = createPwadsToolTip(server);
			break;

		default:
			tooltip = "";
			break;
	}

	QToolTip::showText(QCursor::pos(), tooltip, table);
}

void ServerListHandler::prepareServerTable()
{
	model = createModel();
	sortingProxy = createSortingProxy(model);

	columnHeaderClicked(IDPlayers);
	setupTableProperties(sortingProxy);

	connectTableModelProxySlots();
}

void ServerListHandler::redraw()
{
	model->redrawAll();
}

void ServerListHandler::refreshAll()
{
	for (int i = 0; i < model->rowCount(); ++i)
	{
		Server* serv = model->serverFromList(i);
		serv->refresh();
	}
}

void ServerListHandler::refreshSelected()
{
	QItemSelectionModel* selectionModel = table->selectionModel();
	QModelIndexList indexList = selectionModel->selectedRows();

	for(int i = 0; i < indexList.count(); ++i)
	{
		QModelIndex realIndex = sortingProxy->mapToSource(indexList[i]);
		Server* server = model->serverFromList(realIndex);
		server->refresh();
	}
}

void ServerListHandler::saveColumnsWidthsSettings()
{
	gConfig.doomseeker.serverListColumnState = table->horizontalHeader()->saveState().toBase64();
	gConfig.doomseeker.serverListSortIndex = sortIndex;
	gConfig.doomseeker.serverListSortDirection = sortOrder;
}

QList<Server*> ServerListHandler::selectedServers()
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

void ServerListHandler::serverBegunRefreshing(Server* server)
{
	model->setRefreshing(server);
}

Server *ServerListHandler::serverFromIndex(const QModelIndex &index)
{
	QSortFilterProxyModel* pModel = static_cast<QSortFilterProxyModel*>(table->model());
	QModelIndex indexReal = pModel->mapToSource(index);
	return model->serverFromList(indexReal);
}

void ServerListHandler::serverUpdated(Server *server, int response)
{
	int rowIndex = model->findServerOnTheList(server);
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

void ServerListHandler::setupTableColumnWidths()
{
	QString &headerState = gConfig.doomseeker.serverListColumnState;
	if(headerState.isEmpty())
	{
		for (int i = 0; i < NUM_SERVERLIST_COLUMNS; ++i)
		{
			ServerListColumn* columns = ServerListColumns::columns;
			table->setColumnWidth(i, columns[i].width);
			table->setColumnHidden(i, columns[i].bHidden);
			if(!columns[i].bResizable)
			{
				table->horizontalHeader()->setResizeMode(i, QHeaderView::Fixed);
			}
		}
	}
	else
		table->horizontalHeader()->restoreState(QByteArray::fromBase64(headerState.toAscii()));

	table->horizontalHeader()->setMovable(true);

	if(gConfig.doomseeker.serverListSortIndex >= 0)
	{
		sortIndex = gConfig.doomseeker.serverListSortIndex;
		sortOrder = static_cast<Qt::SortOrder> (gConfig.doomseeker.serverListSortDirection);
	}
}

void ServerListHandler::setupTableProperties(QSortFilterProxyModel* tableModel)
{
	table->setModel(tableModel);
	table->setIconSize(QSize(26, 15));
	// We don't really need a vertical header so lets remove it.
	table->verticalHeader()->hide();
	// Some flags that can't be set from the Designer.
	table->horizontalHeader()->setSortIndicatorShown(true);
	table->horizontalHeader()->setHighlightSections(false);

	table->setMouseTracking(true);

	setupTableColumnWidths();
}

Qt::SortOrder ServerListHandler::swapCurrentSortOrder()
{
	if (sortOrder == Qt::AscendingOrder)
	{
		return Qt::DescendingOrder;
	}
	else
	{
		return Qt::AscendingOrder;
	}
}

void ServerListHandler::tableMiddleClicked(const QModelIndex& index, const QPoint& cursorPosition)
{
	refreshSelected();
}

void ServerListHandler::tableRightClicked(const QModelIndex& index, const QPoint& cursorPosition)
{
	Server* server = serverFromIndex(index);

	ServerListProxyModel* pModel = static_cast<ServerListProxyModel*>(table->model());
	ServerListContextMenu contextMenu(server, pModel->filterInfo());

	QPoint displayPoint = table->viewport()->mapToGlobal(cursorPosition);
	ServerListContextMenu::Result contextMenuResult = contextMenu.exec(displayPoint);

	ServerListFilterInfo filter = contextMenu.serverFilter();
	// 1. This is a bit convoluted, but emitting the serverFilterModified
	//    signal leads to a call to applyFilter() in this class.
	// 2. Since the menu modifies existing server filter, the worst that can
	//    happen is that we set the same filter again.
	emit serverFilterModified(contextMenu.serverFilter());

	switch (contextMenuResult)
	{
		case ServerListContextMenu::DataCopied:
			// Do nothing.
			break;

		case ServerListContextMenu::Join:
			emit serverDoubleClicked(server);
			break;

		case ServerListContextMenu::OpenRemoteConsole:
			new RemoteConsole(server);
			break;

		case ServerListContextMenu::OpenURL:
			QDesktopServices::openUrl(server->webSite());
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

		default:
			QMessageBox::warning(mainWindow, tr("Doomseeker - context menu warning"), tr("Unhandled behavior int ServerListHandler::tableRightClicked()"));
			break;
	}
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

void ServerListHandler::updateSearch(const QString& search)
{
	QRegExp pattern(QString("*") + search + "*", Qt::CaseInsensitive, QRegExp::Wildcard);
	sortingProxy->setFilterRegExp(pattern);
}
