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
 * @brief Remote console protocol interface.
 *
 * Doomseeker can be used to administrate remote server
 * through the server's remote console protocol. Implementation, if provided,
 * is returned through an instance of Server that also needs to be
 * implemented by the plugin.
 */
class MAIN_EXPORT RConProtocol : public QObject
{
	Q_OBJECT

	public:
		virtual ~RConProtocol();

		/**
		 * @brief Whether connection is established or not.
		 *
		 * This is set manually by the plugin implementation through
		 * setConnected() setter. It means that plugin decides on its own
		 * what it means "to be connected" and when exactly the connection
		 * is established.
		 */
		bool isConnected() const;
		/**
		 * @brief List of players present on the server.
		 *
		 * This is a read-only accessor. It's used by Doomseeker to
		 * display players list. It's accessed whenever the
		 * playerListUpdated() signal is emitted. List of players can
		 * be modified through playersMutable() accessor.
		 */
		const QList<Player> &players() const;

	public slots:
		/**
		 * @brief <b>[Pure Virtual]</b> Close connection with the server.
		 *
		 * Disconnects remote console and sets isConnected() state to false.
		 */
		void disconnectFromServer();
		/**
		 * @brief <b>[Pure Virtual]</b> Send command to remote server.
		 *
		 * Command is provided through Doomseeker user interface. It
		 * can be literally anything, and if the plugin needs to
		 * perform any special parsing, it needs to do so on its own.
		 */
		void sendCommand(const QString &cmd);
		/**
		 * @brief <b>[Pure Virtual]</b> Send remote console password.
		 *
		 * Password is provided through Doomseeker user interface.
		 */
		void sendPassword(const QString &password);

	signals:
		/**
		 * @brief Emitted by implementation when connection with the
		 * remote server is closed.
		 */
		void disconnected();
		/**
		 * @brief Emitted by implementation if remote console password
		 * is invalid.
		 *
		 * This ought to be emitted after a call to sendPassword() if
		 * that call fails.
		 */
		void invalidPassword();
		/**
		 * @brief Passes all messages from the remote server that
		 * should be displayed to the user.
		 */
		void messageReceived(const QString &msg);
		/**
		 * @brief Emitted by implementation everytime the list of
		 * players is modified.
		 *
		 * This signal is used to refresh players list. Emit it
		 * whenever you modify players list through playersMutable()
		 * accessor.
		 */
		void playerListUpdated();
		/**
		 * @brief Emitted by implementation when server name changes.
		 *
		 * This signal is used to refresh remote console title.
		 */
		void serverNameChanged(const QString &name);

	protected:
		RConProtocol(ServerPtr server);

		POLYMORPHIC_SETTER_DECLARE(void, RConProtocol, disconnectFromServer, ());
		POLYMORPHIC_SETTER_DECLARE(void, RConProtocol, sendCommand, (const QString &cmd));
		POLYMORPHIC_SETTER_DECLARE(void, RConProtocol, sendPassword, (const QString &password));
		POLYMORPHIC_SETTER_DECLARE(void, RConProtocol, packetReady, ());

		/**
		 * @brief Address of the server to which this remote console
		 * should connect.
		 */
		const QHostAddress &address() const;
		/**
		 * @brief Port of the server to which this remote console
		 * should connect.
		 */
		quint16 port() const;
		/**
		 * @brief Accessor to players list that allows modification.
		 *
		 * Implementation should use this accessor to update players
		 * list. Everytime modification is made, a
		 * playerListUpdated() signal should be emitted.
		 */
		QList<Player> &playersMutable();
		/**
		 * @brief UDP socket that can be used to communicate with
		 * remote server.
		 *
		 * This socket is not connected in any way. It's provided as a
		 * mere convenience. RConProtocol constructor calls bind() on
		 * that socket and its destructor calls close(). Everything
		 * else needs to be programmed by the implementation. If
		 * remote console for your implementation uses a different
		 * means of communication (TCP, HTTP), you may ignore this
		 * socket and implement your own.
		 */
		QUdpSocket &socket();
		/**
		 * @brief Set this to true when connection is successfully
		 * established, set to false upon disconnect.
		 */
		void setConnected(bool b);

		friend class Server;

	protected slots:
		/**
		 * @brief <b>[Pure Virtual] [deprecated]</b>
		 * @deprecated Ignore. Will be removed.
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
