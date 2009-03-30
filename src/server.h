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

		const QString	&getName() const { return name; }
		unsigned short	getScore() const { return score; }
		unsigned short	getPing() const { return ping; }
		bool			isSpectating() const { return spectator; }
		bool			isBot() const { return bot; }
	protected:
		QString			name;
		unsigned short	score;
		unsigned short	ping;
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

		const QString	&getName() { return name;}
		bool			isTeamGame() { return teamgame; }
	protected:
		QString	name;
		bool	teamgame;
};

class Server : public QObject
{
	Q_OBJECT

	public:
		Server(const QHostAddress &address, unsigned short port);
		Server(const Server &other);
		virtual ~Server();

		const QHostAddress	&getAddress() const { return address; }
		const GameMode		&getGameMode() const { return gameMode; }
		const QString		&getMapName() const { return mapName; }
		unsigned short		getMaxClients() const { return maxPlayers > maxClients ? maxPlayers : maxClients; }
		unsigned short		getMaxPlayers() const { return maxPlayers; }
		const QString		&getName() const { return name; }
		int					getNumPlayers() const { return players.size(); }
		int					getNumWads() const { return wads.size(); }
		const Player		&getPlayer(int index) const { return players[index]; }
		unsigned int		getPing() const { return ping; }
		unsigned short		getPort() const { return port; }
		unsigned int		getScoreLimit() const { return scoreLimit; }
		unsigned int		getScore(int team=0) const { return scores[team]; }
		const QString		&getWad(int index) const { return wads[index]; }
		bool				isLocked() const { return locked; }

		void				operator= (const Server &other);

	public slots:
		/**
		 * Updates the server data.
		 */
		virtual void		refresh()=0;

	signals:
		void				banned(const Server *server);
		/**
		 * Emitted by refresh when it is finished and something has changed.
		 */
		void				updated(const Server *server);

	protected:
		QHostAddress		address;
		unsigned short		port;
		unsigned int		ping;

		GameMode			gameMode;
		QString				iwad;
		bool				locked;
		QString				mapName;
		unsigned short		maxClients;
		unsigned short		maxPlayers;
		QString				name;
		QList<Player>		players;
		unsigned int		scoreLimit;
		unsigned int		scores[MAX_TEAMS];
		QStringList			wads;
		QString				webSite;
};

#endif /* __SERVER_H__ */
