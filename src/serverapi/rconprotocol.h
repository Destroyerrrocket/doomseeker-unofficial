//------------------------------------------------------------------------------
// rconprotocol.h
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#ifndef __RCON_PROTOCOL_H_
#define __RCON_PROTOCOL_H_

#include "player.h"
#include <QThread>
#include <QUdpSocket>

class Server;

/**
 *	@brief An abstract interface for a remote console protocol.
 */
class MAIN_EXPORT RConProtocol : public QThread
{
	Q_OBJECT

	public:
		virtual ~RConProtocol();

		bool				isConnected() const { return connected; }
		const QList<Player>	&playerList() const { return players; }

	public slots:
		virtual void	disconnectFromServer()=0;
		virtual void	sendCommand(const QString &cmd)=0;
		virtual void	sendPassword(const QString &password)=0;

	signals:
		void			disconnected();
		void			invalidPassword();
		void			messageReceived(const QString &cmd);
		void			playerListUpdated();
		void			serverNameChanged(const QString &name);

	protected:
		RConProtocol(Server *server);

		bool			connected;
		QList<Player>	players;
		Server			*server;
		QUdpSocket		socket;

		friend class Server;
};

#endif
