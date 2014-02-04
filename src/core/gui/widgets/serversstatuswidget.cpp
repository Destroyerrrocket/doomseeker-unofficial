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

#include "serversstatuswidget.h"
#include "serverapi/masterclient.h"
#include "serverapi/playerslist.h"
#include "serverapi/server.h"

ServersStatusWidget::ServersStatusWidget(const QPixmap &icon, MasterClient *serverList) : QLabel(),
	bMasterIsEnabled(false), icon(icon), numBots(0), numPlayers(0), serverList(serverList)
{
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
	setToolTip(tr("Players-Bots Servers"));

	setIndent(22);
	updateDisplay();

	registerServers();

	connect(serverList, SIGNAL(listUpdated()), this, SLOT(registerServers()));
}

void ServersStatusWidget::addServer(Server *server)
{
	const PlayersList* playersList = server->players();
	numPlayers += playersList->numClients();
	numBots += playersList->numBots();
	updateDisplay();
}

void ServersStatusWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit clicked(serverList);
	}
}

void ServersStatusWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.drawPixmap(2, 2, 18, 18, bMasterIsEnabled ? icon : iconDisabled);
	p.end();

	QLabel::paintEvent(event);
}

void ServersStatusWidget::registerServers()
{
	// Since this is done when the list changes we should reset some values
	numPlayers = 0;
	numBots = 0;

	if (serverList != NULL)
	{
		foreach(Server *server, serverList->servers())
		{
			connect(server, SIGNAL(begunRefreshing(Server *)), this, SLOT(removeServer(Server *)), Qt::DirectConnection);
			connect(server, SIGNAL(updated(Server *, int)), this, SLOT(addServer(Server *)), Qt::DirectConnection);
		}
	}
}

void ServersStatusWidget::removeServer(Server *server)
{
	const PlayersList* playersList = server->players();
	numPlayers -= playersList->numClients();
	numBots -= playersList->numBots();
	updateDisplay();
}

void ServersStatusWidget::setMasterEnabledStatus(bool bEnabled)
{
	this->bMasterIsEnabled = bEnabled;
	updateDisplay();
}

void ServersStatusWidget::updateDisplay()
{
	if (bMasterIsEnabled)
	{
		setText(QString("%1-%2 %3").arg(numPlayers).arg(numBots).arg(serverList != NULL ? serverList->numServers() : 0));
	}
	else
	{
		setText("N/A");
	}
}
