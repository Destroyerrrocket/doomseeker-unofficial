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
#include "serverapi/server.h"
#include <QApplication>
#include <QClipboard>

ServerListContextMenu::ServerListContextMenu(Server* server)
: pServer(server)
{
	initializeMembers();
}

QMenu* ServerListContextMenu::createCopyMenu(QWidget* parent)
{
	QMenu *copyMenu = new QMenu(tr("Copy"), parent);
	copyAddress = copyMenu->addAction(tr("Copy Address"));

	if (!pServer->eMail().isEmpty())
	{
		copyEmail = copyMenu->addAction(tr("Copy E-Mail"));
	}

	if (!pServer->website().isEmpty())
	{
		copyUrl = copyMenu->addAction(tr("Copy URL"));
	}

	copyName = copyMenu->addAction(tr("Copy Name"));

	return copyMenu;
}

void ServerListContextMenu::createMembers()
{
	menu = new QMenu();

	refresh = menu->addAction(tr("Refresh"));
	join = menu->addAction(tr("Join"));
	showJoinCommandLine = menu->addAction(tr("Show join command line"));

	if (!pServer->website().isEmpty() && pServer->isWebsiteURLSafe())
	{
		openUrlInDefaultBrowser = menu->addAction(tr("Open URL in browser"));
	}

	QMenu* copyMenu = createCopyMenu(menu);
	menu->addMenu(copyMenu);

	rcon = NULL;
	if(pServer->hasRcon())
	{
		menu->addSeparator();
		rcon = menu->addAction(tr("Remote Console"));
	}
}

ServerListContextMenu::Result ServerListContextMenu::exec(const QPoint& point)
{
	initializeMembers();
	createMembers();

	QAction* resultAction = menu->exec(point);
	Result result = translateQMenuResult(resultAction);

	// This should take care of deleting all other members.
	delete menu;

	return result;
}

void ServerListContextMenu::initializeMembers()
{
	copyAddress = NULL;
	copyEmail = NULL;
	copyName = NULL;
	copyUrl = NULL;
	join = NULL;
	menu = NULL;
	openUrlInDefaultBrowser = NULL;
	rcon = NULL;
	refresh = NULL;
	showJoinCommandLine = NULL;
}

ServerListContextMenu::Result ServerListContextMenu::translateQMenuResult(QAction* resultAction)
{
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
	else if (resultAction == openUrlInDefaultBrowser && openUrlInDefaultBrowser != NULL)
	{
		return OpenURL;
	}
	else if(resultAction == copyAddress)
	{
		QString addr = QString("%1:%2").arg(pServer->address().toString()).arg(pServer->port());
		QApplication::clipboard()->setText(addr);
		return DataCopied;
	}
	else if (resultAction == copyEmail && copyEmail != NULL)
	{
		QApplication::clipboard()->setText(pServer->eMail());
		return DataCopied;
	}
	else if(resultAction == copyName)
	{
		QApplication::clipboard()->setText(pServer->name());
		return DataCopied;
	}
	else if (resultAction == copyUrl && copyUrl != NULL)
	{
		QApplication::clipboard()->setText(pServer->website());
		return DataCopied;
	}
	else if(resultAction == rcon && rcon != NULL)
	{
		return OpenRemoteConsole;
	}

	return NothingHappened;
}
