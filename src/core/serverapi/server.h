//------------------------------------------------------------------------------
// server.h
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

#ifndef __SERVER_H__
#define __SERVER_H__

#include "serverapi/polymorphism.h"
#include "global.h"

#include <QColor>
#include <QList>
#include <QObject>
#include <QHostAddress>
#include <QHostInfo>
#include <QString>
#include <QMetaType>
#include <QPixmap>
#include <QTime>
#include <QUdpSocket>

class DMFlagsSection;
class EnginePlugin;
class ExeFile;
class GameCVar;
class GameHost;
class GameMode;
class GameClientRunner;
class Player;
class PlayersList;
class PWad;
class RConProtocol;
class TooltipGenerator;

class MAIN_EXPORT Server : public QObject
{
	Q_OBJECT

	friend class ServerPointer;

	public:
		enum Response
		{
			RESPONSE_GOOD, // Data is available
			RESPONSE_TIMEOUT, // Server didn't respond at all
			RESPONSE_WAIT, // Server responded with "wait"
			RESPONSE_BAD, // Probably refreshing too quickly
			RESPONSE_BANNED, // Won't recieve data from this server ever.
			RESPONSE_NO_RESPONSE_YET // "Dummy" response for servers that weren't refreshed yet
		};

		Server(const QHostAddress &address, unsigned short port);
		virtual ~Server();

		// VIRTUALS
		/**
		 * @brief Client executable retriever.
		 */
		virtual ExeFile* clientExe();
		/**
		 * @brief Creates an instance of GameClientRunner's derivative class.
		 *
		 * Gets a pointer to a new instance of GameClientRunner's
		 * descendant (defined by a plugin). Created instance should be deleted
		 * manually by the programmer.
		 */
		virtual GameClientRunner* gameRunner();
		virtual bool hasRcon() const { return false; }
		virtual QList<GameCVar> modifiers() const;
		virtual RConProtocol *rcon() { return NULL; }
		virtual QRgb teamColor(int team) const;
		virtual QString teamName(int team) const;
		/**
		 * This is supposed to return the plugin this Server belongs to.
		 * New instances of EnginePlugin shouldn't be created here. Instead
		 * each plugin should keep a global instance of EnginePlugin (singleton?)
		 * and a pointer to this instance should be returned.
		 */
		virtual EnginePlugin* plugin() const = 0;

		/**
		 * @brief Creates an instance of TooltipGenerator.
		 *
		 * This can be replaced by a plugin to use a custom tooltip generator.
		 *
		 * @return Default behaviour returns default implementation of
		 *         TooltipGenerator.
		 */
		virtual TooltipGenerator* tooltipGenerator() const;
		// END OF VIRTUALS

		void addPlayer(const Player& player);
		const QHostAddress &address() const;
		QString addressWithPort() const;

		/**
		 * @brief IWAD + PWADs.
		 */
		QStringList allWadNames() const;

		/**
		* @brief True if name of any WAD of this server contains given text.
		*/
		bool anyWadnameContains(const QString& text,
			Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

		void clearPlayersList();
		const QString& connectPassword() const;
		const QList<DMFlagsSection>& dmFlags() const;
		const QString& email() const;

		/**
		 * Returns name of the engine for this server, for example: "Skulltag".
		 * This returns name defined by the parent plugin itself,
		 * or "Undefined" string if there is no parent plugin.
		 */
		QString engineName() const;

		const GameMode& gameMode() const;
		const QString& gameVersion() const;

		/**
		 * Returns a string of either the ipaddress:port or hostname:port
		 * depending on if the hostname information has been retrieved, if
		 * reverse lookups is enabled, and the forceAddress parameter.
		 */
		QString hostName(bool forceAddress=false) const;
		const QPixmap &icon() const;

		bool isBroadcastToLAN() const;
		bool isBroadcastToMaster() const;
		bool isCustom() const;
		bool isEmpty() const;
		bool isFull() const;
		bool isKnown() const;

		/**
		 * @brief Passworded or not.
		 */
		bool isLocked() const;

		bool isRandomMapRotation() const;
		bool isRefreshable() const;
		bool isRefreshing() const;

		/**
		 * @brief Secure as in 'secure for players', not 'passworded'.
		 *
		 * When this flag is enabled Doomseeker will draw a shield next to the
		 * engine icon on server list.
		 */
		bool isSecure() const;

		const QString& iwad() const;
		const QString& joinPassword() const;

		Response lastResponse() const;
		/**
		 * @brief Prompts the server to reverse resolve its address to
		 *        a hostname.
		 *
		 * The lookup will be performed asynchronously. When lookup is
		 * finished then updated() signal will be emitted.
		 */
		void lookupHost();

		const QString& map() const;
		const QStringList& mapList() const;
		unsigned short maxClients() const;
		unsigned short maxPlayers() const;
		const QString& motd() const;
		const QString& name() const;
		int numFreeClientSlots() const;
		int numFreeJoinSlots() const;
		int numFreeSpectatorSlots() const;
		int numTotalSlots() const { return maxPlayers() > maxClients() ? maxPlayers() : maxClients(); }
		int numWads() const { return wads().size(); }
		unsigned int ping() const;
		const Player& player(int index) const;
		const PlayersList* players() const;
		unsigned short port() const;
		const QString& rconPassword() const;

		Response readRefreshQueryResponse(QByteArray& data);

		/**
		 * Called when server begins refreshing routine.
		 */
		void refreshStarts();

		/**
		 * Called when server finishes refreshing routine.
		 */
		void refreshStops(Response response);

		unsigned int score(int team=0) const;
		const QList<int>& scores() const;
		unsigned int scoreLimit() const;

		/**
		 * Method called by the refreshing thread. Sends the query
		 * through refreshing thread socket.
		 * @return false if it's impossible to send the query (fail)
		 */
		bool sendRefreshQuery(QUdpSocket* socket);

		void setBroadcastToLAN(bool b);
		void setBroadcastToMaster(bool b);
		void setCustom(bool custom);
		void setConnectPassword(const QString& str);
		void setEmail(const QString& mail);
		void setGameMode(const GameMode& gameMode);
		void setJoinPassword(const QString& str);
		void setMap(const QString& name);
		void setMapList(const QStringList& mapList);
		void setMaxClients(unsigned short i);
		void setMaxPlayers(unsigned short i);
		void setMotd(const QString& message);
		void setName(const QString& name);
		void setPort(unsigned short i);
		void setRandomMapRotation(bool b);
		void setRconPassword(const QString& str);
		void setSkill(unsigned char newSkill);
		void setWebSite(const QString& site);

		unsigned short timeLeft() const;
		unsigned short timeLimit() const;
		unsigned char skill() const;
		const PWad &wad(int index) const;
		const QList<PWad>& wads() const;
		const QString& webSite() const;

	signals:
		void begunRefreshing(Server* server);
		/**
		 * Emitted when a refresh has been completed.  Be sure to check the
		 * response to see if anything has actually changed.
		 * @see Response
		 */
		void updated(Server *server, int response);

	protected:
		/**
		 * @brief <b>[Pure Virtual]</b> Reads response data.
		 *
		 * @return The resposne that should be emitted. Do NOT perform any
		 *         signal emissions from within this functions. This is not
		 *         thread safe and may lead to a crash.
		 */
		Response readRequest(QByteArray &data);
		POLYMORPHIC_SETTER_DECLARE(Response, Server, readRequest, (QByteArray &data));

		/**
		 * @brief <b>[Pure Virtual]</b> Prepares challenge data.
		 *
		 * @return A prepared packet. Return empty buffer to signal error.
		 */
		QByteArray createSendRequest();
		POLYMORPHIC_SETTER_DECLARE(QByteArray, Server, createSendRequest, ());

		void addWad(const QString& wad);
		void clearWads();
		QList<int>& scoresMutable();
		void setGameVersion(const QString& version);
		void setIwad(const QString& iwad);
		void setLocked(bool locked);
		void setPing(unsigned int currentPing);

		/**
		 * @brief Plugins should set this to true to prevent default ping
		 *        calculation.
		 *
		 * This is useful if plugin has its own way of determining ping.
		 */
		void setPingIsSet(bool b);

		void setTimeLeft(unsigned short timeLeft);
		void setTimeLimit(unsigned short timeLimit);
		void setScoreLimit(unsigned int scoreLimit);
		void setSecure(bool bSecureServer);

	private:
		Q_DISABLE_COPY(Server)

		class PrivData;

		static QString teamNames[];

		PrivData* d;

		void clearDMFlags();

		/**
		 * Wrapper function to allow refresher to emit the updated signal.
		 */
		void emitUpdated(int response) { emit updated(this, response); }

		QByteArray createSendRequest_default();
		Response readRequest_default(QByteArray &data);

		void setDmFlags(const QList<DMFlagsSection>& dmFlags);
		void setResponse(Response response);
		void setScores(const QList<int>& scores);
		void setWads(const QList<PWad>& wads);

	private slots:
		void setHostName(QHostInfo host);
};

class MAIN_EXPORT ServerPointer
{
	private:
		void copy(const ServerPointer& copyin)
		{
			ptr = copyin.ptr;
		}

	public:
		Server* ptr;

		ServerPointer() {}
		ServerPointer(Server* s)
		{
			ptr = s;
		}

		ServerPointer(const ServerPointer& copyin)
		{
			copy(copyin);
		}

		ServerPointer& operator=(const ServerPointer& rhs)
		{
			if (this != &rhs)
			{
				copy(rhs);
			}

			return *this;
		}
		~ServerPointer() {}

		bool operator==(const Server* fPtr) const
		{
			return (ptr == fPtr);
		}

		friend bool operator==(const Server* fPtr, const ServerPointer& ref)
		{
			return (fPtr == ref.ptr);
		}

		Server* operator->()
		{
			return ptr;
		}
};

Q_DECLARE_METATYPE(ServerPointer)

#endif /* __SERVER_H__ */
