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

#include <QObject>
#include <QHostAddress>
#include <QString>
#include <QStringList>
#include <QThreadPool>
#include <QThread>
#include <QRunnable>
#include <QMetaType>
#include <QMutex>

#define MAX_TEAMS 4

/**
 * Data structure that holds information about players in a server.
 */
struct Player
{
	public:
		enum PlayerTeam
		{
			TEAM_BLUE,
			TEAM_RED,
			TEAM_GREEN,
			TEAM_GOLD,

			TEAM_NONE = 0xFF
		};

		Player(const QString &name, unsigned short score, unsigned short ping, PlayerTeam team=TEAM_NONE, bool spectator=false, bool bot=false);

		const QString	&name() const { return playerName; }
		short			score() const { return currentScore; }
		unsigned short	ping() const { return currentPing; }
		bool			isSpectating() const { return spectator; }
		bool			isBot() const { return bot; }
		PlayerTeam		teamNum() const { return team; }

		/**
		 * Formats string into HTML format.
		 */
		QString			nameFormatted() const;

		/**
		 * Seeks for characters that are not from the <32; 126> range,
		 * removes them and the characters that appear after them,
		 * then returns new string.
		 */
		QString			nameColorTagsStripped() const;

		QString			teamName() const { return teamName(team); }
		static QString	teamName(int team);

		/**
		 * Colorizes the given string.  Most useful for displaying colored
		 * names.
		 */
		static QString	colorizeString(const QString &str, int def=4);

	protected:
		static QString		teamNames[];
		static const char	colorChart[20][7];

		QString			playerName;
		short			currentScore;
		unsigned short	currentPing;
		bool			spectator;
		bool			bot;
		PlayerTeam		team;
};

/**
 * Data structure that holds information about a servers game mode.
 */
struct GameMode
{
	public:
		// Standard game mode set
		// These should be used in order to keep the names uniform.
		static const GameMode	COOPERATIVE;
		static const GameMode	DEATHMATCH;
		static const GameMode	TEAM_DEATHMATCH;
		static const GameMode	CAPTURE_THE_FLAG;

		/**
		 * @param name Name to display for game mode, this should be fairly short about no longer than "cooperative".
		 */
		GameMode(const QString &name, bool teamgame);

		const QString	&name() const { return modeName;}
		bool			isTeamGame() const { return teamgame; }
	protected:
		QString	modeName;
		bool	teamgame;
};

struct SkillLevel
{
	const QString strName;

	static const int	 numSkillLevels;
	static const QString names[];
};

class Server : public QObject
{
	Q_OBJECT

	public:
		enum Response
		{
			RESPONSE_GOOD,	// Data is available
			RESPONSE_BAD,	// Probably refreshing too quickly
			RESPONSE_BANNED	// Won't recieve data from this server ever.
		};

		Server(const QHostAddress &address, unsigned short port);
		Server(const Server &other);
		virtual ~Server();

		/**
		 * Should return general info about server, like server name, version, email, etc.
		 */
		virtual QString		generalInfoHTML() const =0;

		const QHostAddress	&address() const { return serverAddress; }
		const QStringList	&gameFlags() const { return dmFlags; }
		const GameMode		&gameMode() const { return currentGameMode; }
		bool				isLocked() const { return locked; }
		unsigned int		longestPlayerName() const;
		const QString		&map() const { return mapName; }
		unsigned short		maximumClients() const { return maxPlayers > maxClients ? maxPlayers : maxClients; }
		unsigned short		maximumPlayers() const { return maxPlayers; }
		const QString		&name() const { return serverName; }
		int					numPlayers() const { return players.size(); }
		int					numWads() const { return wads.size(); }
		unsigned int		ping() const { return currentPing; }
		const Player		&player(int index) const { return players[index]; }
		unsigned short		port() const { return serverPort; }
		unsigned int		score(int team=0) const { return scores[team]; }
		unsigned int		scoreLimit() const { return serverScoreLimit; }
		int					teamPlayerCount(int team) const;
		unsigned short		timeLeft() const { return serverTimeLeft; }
		unsigned short		timeLimit() const { return serverTimeLimit; }
		const QString		&wad(int index) const { return wads[index]; }
		const QString		&iwadName() const { return iwad; }
		const QStringList	&pwads() const { return wads; }

		void				operator= (const Server &other);
		virtual void		doRefresh()=0;
		void				startRunning() { bRunning = true; }
		void				stopRunning() { bRunning = false; }

		/**
		 * Returns the thread pool of the refresher.
		 */
		static const QThreadPool	&refresherThreadPool();

		friend class Refresher;
		friend class ServerPointer;

	public slots:
		/**
		 * Updates the server data.
		 */
		void		refresh();

	signals:
		/**
		 * Emitted when a refresh has been completed.  Be sure to check the
		 * response to see if anything has actually changed.
		 * @see Response
		 */
		void				updated(Server *server, int response);

	protected:
		GameMode			currentGameMode;
		unsigned int		currentPing;
		QStringList			dmFlags;
		QString				iwad;
		bool				locked;
		QString				mapName;
		unsigned short		maxClients;
		unsigned short		maxPlayers;
		QList<Player>		players;
		unsigned int		scores[MAX_TEAMS];
		QString				serverName;
		unsigned int		serverScoreLimit;
		unsigned short		serverTimeLeft;
		unsigned short		serverTimeLimit;
		QStringList			wads;
		QString				webSite;

	private:
		/**
		 * This is used to make
		 * sure that refresh() method isn't run on
		 * server that is already refreshing.
		 */
		bool				bRunning;
		QHostAddress		serverAddress;
		unsigned short		serverPort;


};

class ServerRefresher : public QThread, public QRunnable
{
	Q_OBJECT

	private:
		static bool			bGuardianExists;
		bool				bGuardian;
		Server*				parent;

	protected:
		static QThreadPool	threadPool;
		static QThreadPool	guardianThreadPool;
		static QMutex		guardianMutex;

		friend class Server;

	public:
		ServerRefresher(Server* p);

		/**
		 * Creates guardian thread that emits a signal
		 * when all servers are refreshed.
		 */
		void startGuardian();
		void run();

	signals:
		void allServersRefreshed();
};

class ServerPointer
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
