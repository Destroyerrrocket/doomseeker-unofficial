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
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

#include "serversstatuswidget.h"
#include "gui/serverlist.h"
#include "plugins/engineplugin.h"
#include "serverapi/masterclient.h"
#include "serverapi/playerslist.h"
#include "serverapi/server.h"

ServersStatusWidget::ServersStatusWidget(const EnginePlugin *plugin, const ServerList *serverList)
	: QLabel(), enabled(false), icon(plugin->icon()),
	  numBots(0), numPlayers(0), numServers(0), numRefreshing(0)
{
	this->plugin = plugin;

	this->serverList = serverList;
	this->connect(serverList, SIGNAL(serverRegistered(ServerPtr)),
		SLOT(registerServerIfSamePlugin(ServerPtr)));
	this->connect(serverList, SIGNAL(serverDeregistered(ServerPtr)),
		SLOT(deregisterServerIfSamePlugin(ServerPtr)));

	// Transform icon to grayscale format for disabled appearance
	QImage iconImage = icon.toImage();
	int width = iconImage.width();
	int height = iconImage.height();
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			QRgb pixel = iconImage.pixel(x, y);
			int alpha = qAlpha(pixel);
			int gray = qGray(pixel);

			pixel = qRgba(gray, gray, gray, alpha);

			iconImage.setPixel(x, y, pixel);
		}
	}

	iconDisabled = QPixmap::fromImage(iconImage);

	// Have an inset frame unless we're on the Mac
#ifndef Q_OS_MAC
	setFrameShape(QFrame::Panel);
	setFrameShadow(QFrame::Sunken);
#else
	setFrameShape(QFrame::NoFrame);
#endif

	setFixedHeight(22);
	setToolTip(tr("Players-Bots Servers Refreshed%"));

	setIndent(22);
	updateDisplay();

	resetStatus();
}

void ServersStatusWidget::increaseCountersAndUpdateDisplay(const ServerPtr &server)
{
	increaseCounters(server);
	updateDisplay();
}

void ServersStatusWidget::increaseCounters(ServerPtr server)
{
	const PlayersList &players = server->players();
	numPlayers += players.numClients();
	numBots += players.numBots();
}

void ServersStatusWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit clicked(plugin);
	}
}

void ServersStatusWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.drawPixmap(2, 2, 18, 18, enabled ? icon : iconDisabled);
	p.end();

	QLabel::paintEvent(event);
}

void ServersStatusWidget::resetStatus()
{
	// Since this is done when the list changes we should reset some values
	numPlayers = 0;
	numBots = 0;

	if (serverList != NULL)
	{
		foreach(ServerPtr server, serverList->serversForPlugin(plugin))
		{
			increaseCounters(server);
		}
	}
	updateDisplay();
}

void ServersStatusWidget::registerServerIfSamePlugin(ServerPtr server)
{
	if (server->plugin() == plugin)
	{
		registerServer(server);
	}
}

void ServersStatusWidget::registerServer(ServerPtr server)
{
	this->connect(server.data(), SIGNAL(begunRefreshing(ServerPtr)),
		SLOT(decreaseCountersAndUpdateDisplay(ServerPtr)));
	this->connect(server.data(), SIGNAL(begunRefreshing(ServerPtr)),
		SLOT(increaseRefreshingCountAndUpdateDisplay()));
	this->connect(server.data(), SIGNAL(updated(ServerPtr, int)),
		SLOT(increaseCountersAndUpdateDisplay(ServerPtr)));
	this->connect(server.data(), SIGNAL(updated(ServerPtr, int)),
		SLOT(decreaseRefreshingCountAndUpdateDisplay()));
	increaseCountersAndUpdateDisplay(server);
	++numServers;
	if (server->isRefreshing())
	{
		++numRefreshing;
	}
}

void ServersStatusWidget::deregisterServerIfSamePlugin(const ServerPtr &server)
{
	if (server->plugin() == plugin)
	{
		server->disconnect(this);
		if (!server->isRefreshing())
		{
			// A refreshing server has already decreased its counters
			// so let's not decrease it twice or errors will happen.
			decreaseCountersAndUpdateDisplay(server);
		}
		else
		{
			--numRefreshing;
		}
		--numServers;
		updateDisplay();
	}
}

void ServersStatusWidget::decreaseCountersAndUpdateDisplay(const ServerPtr &server)
{
	const PlayersList &players = server->players();
	numPlayers -= players.numClients();
	numBots -= players.numBots();
	updateDisplay();
}

void ServersStatusWidget::decreaseRefreshingCountAndUpdateDisplay()
{
	--numRefreshing;
	updateDisplay();
}

void ServersStatusWidget::increaseRefreshingCountAndUpdateDisplay()
{
	++numRefreshing;
	updateDisplay();
}

QString ServersStatusWidget::refreshedPercentAsText() const
{
	if (numServers == 0)
	{
		return tr("N/A");
	}
	else
	{
		return tr("%1%").arg(refreshedPercent());
	}
}

unsigned ServersStatusWidget::refreshedPercent() const
{
	if (numRefreshing == 0 || numServers == 0)
	{
		return 100;
	}
	else
	{
		float refreshingFactor = static_cast<float>(numRefreshing) /
			static_cast<float>(numServers);
		return static_cast<unsigned>(floor(100.0 - 100.0 * refreshingFactor));
	}
}

void ServersStatusWidget::setMasterEnabledStatus(bool bEnabled)
{
	this->enabled = bEnabled;
	updateDisplay();
}

void ServersStatusWidget::updateDisplay()
{
	if (enabled)
	{
		setText(QString("%1-%2 %3 %4").arg(numPlayers).arg(numBots)
			.arg(numServers).arg(refreshedPercentAsText()));
	}
	else
	{
		setText("N/A");
	}
}
