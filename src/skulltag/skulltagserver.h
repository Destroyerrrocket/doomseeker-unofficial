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
			SQF_NAME =				0x00000001,
			SQF_URL =				0x00000002,
			SQF_EMAIL =				0x00000004,
			SQF_MAPNAME =			0x00000008,
			SQF_MAXCLIENTS =		0x00000010,
			SQF_MAXPLAYERS =		0x00000020,
			SQF_PWADS =				0x00000040,
			SQF_GAMETYPE =			0x00000080,
			SQF_GAMENAME =			0x00000100,
			SQF_IWAD =				0x00000200,
			SQF_FORCEPASSWORD =		0x00000400,
			SQF_FORCEJOINPASSWORD =	0x00000800,
			SQF_GAMESKILL =			0x00001000,
			SQF_BOTSKILL =			0x00002000,
			SQF_DMFLAGS =			0x00004000,
			SQF_LIMITS =			0x00010000,
			SQF_TEAMDAMAGE =		0x00020000,
			SQF_TEAMSCORES =		0x00040000,
			SQF_NUMPLAYERS =		0x00080000,
			SQF_PLAYERDATA =		0x00100000,
			SQF_TEAMINFO_NUMBER =	0x00200000,
			SQF_TEAMINFO_NAME =		0x00400000,
			SQF_TEAMINFO_COLOR =	0x00800000,
			SQF_TEAMINFO_SCORE =	0x01000000,
			SQF_TESTING_SERVER =	0x02000000,

			SQF_STANDARDQUERY =		SQF_NAME|SQF_URL|SQF_EMAIL|SQF_MAPNAME|SQF_MAXCLIENTS|SQF_MAXPLAYERS|SQF_PWADS|SQF_GAMETYPE|SQF_IWAD|SQF_FORCEPASSWORD|SQF_FORCEJOINPASSWORD|SQF_DMFLAGS|SQF_LIMITS|SQF_NUMPLAYERS|SQF_PLAYERDATA|SQF_TEAMINFO_NUMBER|SQF_TEAMINFO_NAME|SQF_TEAMINFO_SCORE|SQF_GAMESKILL|SQF_TESTING_SERVER
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
		static const QPixmap	ICON;

		SkulltagServer(const QHostAddress &address, unsigned short port);

		QPixmap			icon() const;
		QString			clientBinary() const;
		void			connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const;
		QString			teamName(int team) const;

		void			doRefresh();

	protected:
		bool			buckshot;
		bool			instagib;
		bool			testingServer;

		float			teamDamage;

		unsigned short	botSkill;
		unsigned short	skill;

		unsigned short	duelLimit;
		unsigned short	fragLimit;
		unsigned short	pointLimit;
		unsigned short	winLimit;

		unsigned int	numTeams;
		TeamInfo		teamInfo[ST_MAX_TEAMS];

		bool			readRequest(QByteArray &data);
		bool			sendRequest(QByteArray &data);

		QString			testingArchive;
};

#endif /* __SKULLTAGSERVER_H__ */
