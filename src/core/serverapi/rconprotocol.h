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

#include "serverapi/polymorphism.h"
#include "serverapi/serverptr.h"
#include "player.h"
#include <QThread>
#include <QUdpSocket>

/**
 * @ingroup group_pluginapi
 * @brief An abstract interface for a remote console protocol.
 */
class MAIN_EXPORT RConProtocol : public QObject
{
	Q_OBJECT

	public:
		virtual ~RConProtocol();

		bool isConnected() const;
		const QList<Player> &players() const;

	public slots:
		/**
		 * @brief <b>[Pure Virtual]</b>
		 */
		void disconnectFromServer();
		/**
		 * @brief <b>[Pure Virtual]</b>
		 */
		void sendCommand(const QString &cmd);
		/**
		 * @brief <b>[Pure Virtual]</b>
		 */
		void sendPassword(const QString &password);

	signals:
		void disconnected();
		void invalidPassword();
		void messageReceived(const QString &cmd);
		void playerListUpdated();
		void serverNameChanged(const QString &name);

	protected:
		RConProtocol(ServerPtr server);

		POLYMORPHIC_SETTER_DECLARE(void, RConProtocol, disconnectFromServer, ());
		POLYMORPHIC_SETTER_DECLARE(void, RConProtocol, sendCommand, (const QString &cmd));
		POLYMORPHIC_SETTER_DECLARE(void, RConProtocol, sendPassword, (const QString &password));
		POLYMORPHIC_SETTER_DECLARE(void, RConProtocol, packetReady, ());

		const QHostAddress &address() const;
		quint16 port() const;
		QList<Player> &playersMutable();
		QUdpSocket &socket();
		void setConnected(bool b);

		friend class Server;

	protected slots:
		/**
		 * @brief <b>[Pure Virtual]</b>
		 */
		void packetReady();

	private:
		class PrivData;
		PrivData* d;

		void disconnectFromServer_default();
		void sendCommand_default(const QString &cmd);
		void sendPassword_default(const QString &password);
		void packetReady_default();
};

#endif
