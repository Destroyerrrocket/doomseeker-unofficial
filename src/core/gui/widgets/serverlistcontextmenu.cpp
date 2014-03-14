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
#include <cassert>

class ServerListContextMenu::PrivData
{
	public:
		QAction *clearAdditionalSorting;
		QAction *removeAdditionalSortingForColumn;
		QAction *sortAdditionallyAscending;
		QAction *sortAdditionallyDescending;
		QModelIndex modelIndex;
		ServerListFilterInfo serverFilter;
		ServerListHandler *parent;
};

ServerListContextMenu::ServerListContextMenu(ServerPtr server, const ServerListFilterInfo& filter,
	const QModelIndex &modelIndex, ServerListHandler *parent)
: QObject(parent), pServer(server)
{
	d = new PrivData();
	d->parent = parent;
	d->serverFilter = filter;
	d->modelIndex = modelIndex;
	initializeMembers();
	createMembers();
}

ServerListContextMenu::~ServerListContextMenu()
{
	delete menu;
	delete d;
}

QMenu* ServerListContextMenu::createCopyMenu(QWidget* parent)
{
	QMenu *copyMenu = new QMenu(tr("Copy"), parent);
	copyAddress = copyMenu->addAction(tr("Copy Address"));

	if (!pServer->email().isEmpty())
	{
		copyEmail = copyMenu->addAction(tr("Copy E-Mail"));
	}

	if (!pServer->webSite().isEmpty())
	{
		copyUrl = copyMenu->addAction(tr("Copy URL"));
	}

	copyName = copyMenu->addAction(tr("Copy Name"));

	return copyMenu;
}

void ServerListContextMenu::createMembers()
{
	menu = new QMenu();
	this->connect(menu, SIGNAL(aboutToHide()), SIGNAL(aboutToHide()));
	this->connect(menu, SIGNAL(triggered(QAction*)), SIGNAL(triggered(QAction*)));

	refresh = menu->addAction(tr("Refresh"));
	join = menu->addAction(tr("Join"));
	showJoinCommandLine = menu->addAction(tr("Show join command line"));

	// Website.
	const QString& webSite = pServer->webSite();
	bool bShouldAllowOpenUrl = !webSite.isEmpty() && Strings::isUrlSafe(webSite);

	if (bShouldAllowOpenUrl)
	{
		openUrlInDefaultBrowser = menu->addAction(tr("Open URL in browser"));
	}

	// Copy menu.
	QMenu* copyMenu = createCopyMenu(menu);
	menu->addMenu(copyMenu);

	// Filter builder.
	filterBuilder = new ServerFilterBuilderMenu(*pServer, d->serverFilter, menu);
	if (pServer->isKnown() && !filterBuilder->isEmpty())
	{
		menu->addMenu(filterBuilder);
	}

	rcon = NULL;
	if(pServer->hasRcon())
	{
		menu->addSeparator();
		rcon = menu->addAction(tr("Remote Console"));
	}

	// Sorts.
	menu->addSeparator();
	if (!d->parent->isSortingByColumn(d->modelIndex.column()))
	{
		d->sortAdditionallyAscending = menu->addAction(tr("Sort additionally ascending"));
		d->sortAdditionallyDescending = menu->addAction(tr("Sort additionally descending"));
	}
	if (d->parent->isSortingAdditionallyByColumn(d->modelIndex.column()))
	{
		d->removeAdditionalSortingForColumn = menu->addAction(
			tr("Remove additional sorting for column"));
	}
	if (d->parent->isAnyColumnSortedAdditionally())
	{
		d->clearAdditionalSorting = menu->addAction(tr("Clear additional sorting"));
	}
}

void ServerListContextMenu::initializeMembers()
{
	d->clearAdditionalSorting = NULL;
	d->removeAdditionalSortingForColumn = NULL;
	d->sortAdditionallyAscending = NULL;
	d->sortAdditionallyDescending = NULL;
	copyAddress = NULL;
	copyEmail = NULL;
	copyName = NULL;
	copyUrl = NULL;
	filterBuilder = NULL;
	join = NULL;
	menu = NULL;
	openUrlInDefaultBrowser = NULL;
	rcon = NULL;
	refresh = NULL;
	showJoinCommandLine = NULL;
}

const QModelIndex &ServerListContextMenu::modelIndex() const
{
	return d->modelIndex;
}

void ServerListContextMenu::popup(const QPoint& point)
{
	menu->popup(point);
}

ServerPtr ServerListContextMenu::server() const
{
	return pServer;
}

const ServerListFilterInfo& ServerListContextMenu::serverFilter() const
{
	assert(filterBuilder);
	return filterBuilder->filter();
}

ServerListContextMenu::Result ServerListContextMenu::translateQMenuResult(QAction* resultAction)
{
	if (resultAction == NULL)
	{
		return NothingHappened;
	}

	// Now perform checks against menu items.
	if(resultAction == refresh)
	{
		return Refresh;
	}
	else if(resultAction == join)
	{
		return Join;
	}
	else if (resultAction == showJoinCommandLine)
	{
		return ShowJoinCommandLine;
	}
	else if (resultAction == openUrlInDefaultBrowser)
	{
		return OpenURL;
	}
	else if(resultAction == copyAddress)
	{
		QString addr = QString("%1:%2").arg(pServer->address().toString()).arg(pServer->port());
		QApplication::clipboard()->setText(addr);
		return DataCopied;
	}
	else if (resultAction == copyEmail)
	{
		QApplication::clipboard()->setText(pServer->email());
		return DataCopied;
	}
	else if(resultAction == copyName)
	{
		QApplication::clipboard()->setText(pServer->name());
		return DataCopied;
	}
	else if (resultAction == copyUrl)
	{
		QApplication::clipboard()->setText(pServer->webSite());
		return DataCopied;
	}
	else if(resultAction == rcon)
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
