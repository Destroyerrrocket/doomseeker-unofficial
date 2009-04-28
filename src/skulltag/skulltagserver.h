//------------------------------------------------------------------------------
// skulltagserver.h
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

#ifndef __SKULLTAGSERVER_H__
#define __SKULLTAGSERVER_H__

#include <QString>
#include <QColor>
#include <QTime>

#include "server.h"

#define	NUM_SKULLTAG_GAME_MODES 16
#define ST_MAX_TEAMS			4

class SkulltagServer;

struct TeamInfo
{
	public:
		TeamInfo(const QString &name=QObject::tr("<< Unknown >>"), const QColor &color=QColor(0,0,0), unsigned int score=0);

		const QString	&name() const { return teamName; }
		const QColor	&color() const { return teamColor; }
		unsigned int	score() const { return teamScore; }

	protected:
		void			setName(const QString &name) { teamName = name; }
		void			setColor(const QColor &color) { teamColor = color; }
		void			setScore(int score) { teamScore = score; }

		friend class	SkulltagServer;

	private:
		QString			teamName;
		QColor			teamColor;
		unsigned int	teamScore;
};

class SkulltagServer : public Server
{
	Q_OBJECT

	public:


		enum SkulltagQueryFlags
		{
			SQF_NAME =				0x0000001,
			SQF_URL =				0x0000002,
			SQF_EMAIL =				0x0000004,
			SQF_MAPNAME =			0x0000008,
			SQF_MAXCLIENTS =		0x0000010,
			SQF_MAXPLAYERS =		0x0000020,
			SQF_PWADS =				0x0000040,
			SQF_GAMETYPE =			0x0000080,
			SQF_GAMENAME =			0x0000100,
			SQF_IWAD =				0x0000200,
			SQF_FORCEPASSWORD =		0x0000400,
			SQF_FORCEJOINPASSWORD =	0x0000800,
			SQF_GAMESKILL =			0x0001000,
			SQF_BOTSKILL =			0x0002000,
			SQF_DMFLAGS =			0x0004000,
			SQF_LIMITS =			0x0010000,
			SQF_TEAMDAMAGE =		0x0020000,
			SQF_TEAMSCORES =		0x0040000,
			SQF_NUMPLAYERS =		0x0080000,
			SQF_PLAYERDATA =		0x0100000,
			SQF_TEAMINFO_NUMBER =	0x0200000,
			SQF_TEAMINFO_NAME =		0x0400000,
			SQF_TEAMINFO_COLOR =	0x0800000,
			SQF_TEAMINFO_SCORE =	0x1000000,

			SQF_STANDARDQUERY =		SQF_NAME|SQF_URL|SQF_EMAIL|SQF_MAPNAME|SQF_MAXCLIENTS|SQF_MAXPLAYERS|SQF_PWADS|SQF_GAMETYPE|SQF_IWAD|SQF_FORCEPASSWORD|SQF_FORCEJOINPASSWORD|SQF_DMFLAGS|SQF_LIMITS|SQF_NUMPLAYERS|SQF_PLAYERDATA|SQF_TEAMINFO_NUMBER|SQF_TEAMINFO_NAME|SQF_TEAMINFO_SCORE|SQF_GAMESKILL
		};

		enum SkulltagGameMode
		{
			GAMEMODE_COOPERATIVE,
			GAMEMODE_SURVIVAL,
			GAMEMODE_INVASION,
			GAMEMODE_DEATHMATCH,
			GAMEMODE_TEAMPLAY,
			GAMEMODE_DUEL,
			GAMEMODE_TERMINATOR,
			GAMEMODE_LASTMANSTANDING,
			GAMEMODE_TEAMLMS,
			GAMEMODE_POSSESSION,
			GAMEMODE_TEAMPOSSESSION,
			GAMEMODE_TEAMGAME,
			GAMEMODE_CTF,
			GAMEMODE_ONEFLAGCTF,
			GAMEMODE_SKULLTAG,
			GAMEMODE_DOMINATION
		};

		/**
		 * Lists all DMFlags, DMFlags2, and CompatFlags in order.
		 */
		static const QString	DMFLAGS[96];
		static const GameMode	GAME_MODES[NUM_SKULLTAG_GAME_MODES];

		SkulltagServer(const QHostAddress &address, unsigned short port);

		QString			gameInfoTableHTML() const;
		QString			generalInfoHTML() const;
		QString			playerTableHTML() const;

		void			doRefresh();

	protected:
		QString			email;
		QString			version;

		bool			buckshot;
		bool			instagib;

		float			teamDamage;

		unsigned short	botSkill;
		unsigned short	skill;

		unsigned short	duelLimit;
		unsigned short	fragLimit;
		unsigned short	pointLimit;
		unsigned short	winLimit;

		unsigned int	numTeams;
		TeamInfo		teamInfo[ST_MAX_TEAMS];

		void 			additionalServerInfo(QList<ServerInfo>* baseList) const;
		bool			readRequest(QByteArray &data, QTime &time);
		bool			sendRequest(QByteArray &data);

		/**
		 * This will return absolutely nothing if the list in the first argument is empty.
		 */
        QString 		spawnPartOfPlayerTable(QList<const Player*>, QString status, int colspan, bool isTeamgame, bool bAppendEmptyRowAtBeginning) const;
};

#endif /* __SKULLTAGSERVER_H__ */
