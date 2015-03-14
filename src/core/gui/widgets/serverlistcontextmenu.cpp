//------------------------------------------------------------------------------
// serverlistcontextmenu.cpp
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
#include "serverlistcontextmenu.h"

#include "gui/entity/serverlistfilterinfo.h"
#include "gui/widgets/serverfilterbuildermenu.h"
#include "gui/serverlist.h"
#include "serverapi/server.h"
#include "strings.h"
#include <QApplication>
#include <QClipboard>
#include <QModelIndex>
#include <cassert>

DClass<ServerListContextMenu>
{
public:
	QAction *copyAddress;
	QAction *copyEmail;
	QAction *copyName;
	QAction *copyUrl;
	QAction *clearAdditionalSorting;
	QAction *findMissingWads;
	QAction *join;
	QAction *openUrlInDefaultBrowser;
	QAction *rcon;
	QAction *refresh;
	QAction *removeAdditionalSortingForColumn;
	QAction *showJoinCommandLine;
	QAction *sortAdditionallyAscending;
	QAction *sortAdditionallyDescending;
	QMenu *menu;
	ServerFilterBuilderMenu *filterBuilder;
	ServerPtr pServer;
	QModelIndex modelIndex;
	ServerListFilterInfo serverFilter;
	ServerListHandler *parent;
};

DPointered(ServerListContextMenu)

ServerListContextMenu::ServerListContextMenu(ServerPtr server, const ServerListFilterInfo& filter,
	const QModelIndex &modelIndex, ServerListHandler *parent)
: QObject(parent)
{
	d->pServer = server;
	d->parent = parent;
	d->serverFilter = filter;
	d->modelIndex = modelIndex;
	initializeMembers();
	createMembers();
}

ServerListContextMenu::~ServerListContextMenu()
{
	delete d->menu;
}

QMenu* ServerListContextMenu::createCopyMenu(QWidget* parent)
{
	QMenu *copyMenu = new QMenu(tr("Copy"), parent);
	d->copyAddress = copyMenu->addAction(tr("Copy Address"));

	if (!d->pServer->email().isEmpty())
	{
		d->copyEmail = copyMenu->addAction(tr("Copy E-Mail"));
	}

	if (!d->pServer->webSite().isEmpty())
	{
		d->copyUrl = copyMenu->addAction(tr("Copy URL"));
	}

	d->copyName = copyMenu->addAction(tr("Copy Name"));

	return copyMenu;
}

void ServerListContextMenu::createMembers()
{
	d->menu = new QMenu();
	this->connect(d->menu, SIGNAL(aboutToHide()), SIGNAL(aboutToHide()));
	this->connect(d->menu, SIGNAL(triggered(QAction*)), SIGNAL(triggered(QAction*)));

	d->refresh = d->menu->addAction(tr("Refresh"));
	d->join = d->menu->addAction(tr("Join"));
	d->showJoinCommandLine = d->menu->addAction(tr("Show join command line"));
	d->findMissingWads = d->menu->addAction(tr("Find missing WADs"));

	// Website.
	const QString& webSite = d->pServer->webSite();
	bool bShouldAllowOpenUrl = !webSite.isEmpty() && Strings::isUrlSafe(webSite);

	if (bShouldAllowOpenUrl)
	{
		d->openUrlInDefaultBrowser = d->menu->addAction(tr("Open URL in browser"));
	}

	// Copy menu.
	QMenu* copyMenu = createCopyMenu(d->menu);
	d->menu->addMenu(copyMenu);

	// Filter builder.
	d->filterBuilder = new ServerFilterBuilderMenu(*d->pServer, d->serverFilter, d->menu);
	if (d->pServer->isKnown() && !d->filterBuilder->isEmpty())
	{
		d->menu->addMenu(d->filterBuilder);
	}

	d->rcon = NULL;
	if(d->pServer->hasRcon())
	{
		d->menu->addSeparator();
		d->rcon = d->menu->addAction(tr("Remote Console"));
	}

	// Sorts.
	d->menu->addSeparator();
	if (!d->parent->isSortingByColumn(d->modelIndex.column()))
	{
		d->sortAdditionallyAscending = d->menu->addAction(tr("Sort additionally ascending"));
		d->sortAdditionallyDescending = d->menu->addAction(tr("Sort additionally descending"));
	}
	if (d->parent->isSortingAdditionallyByColumn(d->modelIndex.column()))
	{
		d->removeAdditionalSortingForColumn = d->menu->addAction(
			tr("Remove additional sorting for column"));
	}
	if (d->parent->isAnyColumnSortedAdditionally())
	{
		d->clearAdditionalSorting = d->menu->addAction(tr("Clear additional sorting"));
	}
}

void ServerListContextMenu::initializeMembers()
{
	d->clearAdditionalSorting = NULL;
	d->removeAdditionalSortingForColumn = NULL;
	d->sortAdditionallyAscending = NULL;
	d->sortAdditionallyDescending = NULL;
	d->copyAddress = NULL;
	d->copyEmail = NULL;
	d->copyName = NULL;
	d->copyUrl = NULL;
	d->filterBuilder = NULL;
	d->findMissingWads = NULL;
	d->join = NULL;
	d->menu = NULL;
	d->openUrlInDefaultBrowser = NULL;
	d->rcon = NULL;
	d->refresh = NULL;
	d->showJoinCommandLine = NULL;
}

const QModelIndex &ServerListContextMenu::modelIndex() const
{
	return d->modelIndex;
}

void ServerListContextMenu::popup(const QPoint& point)
{
	d->menu->popup(point);
}

ServerPtr ServerListContextMenu::server() const
{
	return d->pServer;
}

const ServerListFilterInfo& ServerListContextMenu::serverFilter() const
{
	assert(d->filterBuilder);
	return d->filterBuilder->filter();
}

ServerListContextMenu::Result ServerListContextMenu::translateQMenuResult(QAction* resultAction)
{
	if (resultAction == NULL)
	{
		return NothingHappened;
	}

	// Now perform checks against menu items.
	if(resultAction == d->refresh)
	{
		return Refresh;
	}
	else if(resultAction == d->join)
	{
		return Join;
	}
	else if (resultAction == d->showJoinCommandLine)
	{
		return ShowJoinCommandLine;
	}
	else if (resultAction == d->openUrlInDefaultBrowser)
	{
		return OpenURL;
	}
	else if(resultAction == d->copyAddress)
	{
		QString addr = QString("%1:%2").arg(d->pServer->address().toString()).arg(d->pServer->port());
		QApplication::clipboard()->setText(addr);
		return DataCopied;
	}
	else if (resultAction == d->copyEmail)
	{
		QApplication::clipboard()->setText(d->pServer->email());
		return DataCopied;
	}
	else if(resultAction == d->copyName)
	{
		QApplication::clipboard()->setText(d->pServer->name());
		return DataCopied;
	}
	else if (resultAction == d->copyUrl)
	{
		QApplication::clipboard()->setText(d->pServer->webSite());
		return DataCopied;
	}
	else if(resultAction == d->findMissingWads)
	{
		return FindMissingWADs;
	}
	else if(resultAction == d->rcon)
	{
		return OpenRemoteConsole;
	}
	else if(resultAction == d->sortAdditionallyAscending)
	{
		return SortAdditionallyAscending;
	}
	else if(resultAction == d->sortAdditionallyDescending)
	{
		return SortAdditionallyDescending;
	}
	else if(resultAction == d->clearAdditionalSorting)
	{
		return ClearAdditionalSorting;
	}
	else if(resultAction == d->removeAdditionalSortingForColumn)
	{
		return RemoveAdditionalSortingForColumn;
	}

	return NothingHappened;
}
