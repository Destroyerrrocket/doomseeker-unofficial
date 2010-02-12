//------------------------------------------------------------------------------
// serversstatuswidget.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>

#include "serversstatuswidget.h"
#include "serverapi/playerslist.h"

ServersStatusWidget::ServersStatusWidget(const QPixmap &icon, const MasterClient *serverList) : QLabel(),
	icon(icon), numBots(0), numPlayers(0), serverList(serverList)
{
	setFrameShape(QFrame::Panel);
	setFrameShadow(QFrame::Sunken);
	setFixedHeight(22);
	setToolTip(tr("Players-Bots Servers"));

	setIndent(22);
	updateDisplay();

	registerServers();

	connect(serverList, SIGNAL(listUpdated()), this, SLOT(registerServers()));
}

void ServersStatusWidget::addServer(Server *server)
{
	const PlayersList* playersList = server->playersList();
	numPlayers += playersList->numClients();
	numBots += playersList->numBots();
	updateDisplay();
}

void ServersStatusWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.drawPixmap(2, 2, 18, 18, icon);

	QLabel::paintEvent(event);
}

void ServersStatusWidget::registerServers()
{
	// Since this is done when the list changes we should reset some values
	numPlayers = 0;
	numBots = 0;

	foreach(Server *server, serverList->serverList())
	{
		connect(server, SIGNAL(begunRefreshing(Server *)), this, SLOT(removeServer(Server *)));
		connect(server, SIGNAL(updated(Server *, int)), this, SLOT(addServer(Server *)));
	}
}

void ServersStatusWidget::removeServer(Server *server)
{
	const PlayersList* playersList = server->playersList();
	numPlayers -= playersList->numClients();
	numBots -= playersList->numBots();
	updateDisplay();
}

void ServersStatusWidget::updateDisplay()
{
	setText(QString("%1-%2 %3").arg(numPlayers).arg(numBots).arg(serverList->numServers()));
}
