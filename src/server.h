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
		unsigned short	score() const { return currentScore; }
		unsigned short	ping() const { return currentPing; }
		bool			isSpectating() const { return spectator; }
		bool			isBot() const { return bot; }
	protected:
		QString			playerName;
		unsigned short	currentScore;
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

class Server : public QObject
{
	Q_OBJECT

	public:
		Server(const QHostAddress &address, unsigned short port);
		Server(const Server &other);
		virtual ~Server();

		const QHostAddress	&address() const { return serverAddress; }
		const QStringList	&gameFlags() const { return dmFlags; }
		const GameMode		&gameMode() const { return currentGameMode; }
		bool				isLocked() const { return locked; }
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
		unsigned short		timeLeft() const { return serverTimeLeft; }
		unsigned short		timeLimit() const { return serverTimeLimit; }
		const QString		&wad(int index) const { return wads[index]; }

		void				operator= (const Server &other);
		virtual void		doRefresh()=0;

	public slots:
		/**
		 * Updates the server data.
		 */
		void		refresh();
		

	signals:
		void				banned(const Server *server);
		/**
		 * Emitted by refresh when it is finished and something has changed.
		 */
		void				updated(const Server *server);

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
		QHostAddress		serverAddress;
		unsigned short		serverPort;
		
		class Refresher;
		friend class Refresher;		
};

class Server::Refresher : public QThread, public QRunnable
{
	private:
		Server* parent;
			
	public:
		Refresher(Server* p);
		void run();
};

#endif /* __SERVER_H__ */
