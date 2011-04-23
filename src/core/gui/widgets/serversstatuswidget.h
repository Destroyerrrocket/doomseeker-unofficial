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

#ifndef __SERVERSSTATUSWIDGET_H__
#define __SERVERSSTATUSWIDGET_H__

#include <QLabel>

class MasterClient;
class Server;

class ServersStatusWidget : public QLabel
{
	Q_OBJECT

	public:
		ServersStatusWidget(const QPixmap &icon, const MasterClient *serverList);

	public slots:
		void	updateDisplay();

	protected:
		void	paintEvent(QPaintEvent *event);

		QPixmap				icon;
		unsigned int		numBots;
		unsigned int		numPlayers;
		const MasterClient	*serverList;

	protected slots:
		void	addServer(Server *server);
		void	registerServers();
		void	removeServer(Server *server);
};

#endif /* __SERVERSSTATUSWIDGET_H__ */
