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
		/**
		 *	If this is a normal server simple path to executable file is produced.
		 *	If this is a testing server, a shell script will be produced if necessary and
		 *	path to this shell script will be returned.
		 */
		QString			clientBinary(QString& error) const;
		QString			clientBinarysDirectory() const;
		void			connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const;
		RConProtocol	*rcon();
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
		/**
		 *	Creates Unix .sh file or Windows .bat file to
		 *	launch client for this server. Returns true if the file
		 *	already exists.
		 *	@param versionDir - convenience parameter. This is the directory
		 *		where testing package was unpacked. This path was
		 *		already created in clientBinary() method so let's reuse it.
		 *	@param [out] fullPathToFile - path to created script file
		 *	@param [out] error - error if return == false
		 *	@return false if fail
		 */
		bool			spawnTestingBatchFile(const QString& versionDir, QString& fullPathToFile, QString& error) const;

		QString			testingArchive;
};

class SkulltagRConProtocol : public RConProtocol
{
	Q_OBJECT

	private:
		enum
		{
			SVRCU_PLAYERDATA = 0,
			SVRCU_ADMINCOUNT,
			SVRCU_MAP,

			SVRC_OLDPROTOCOL = 32,
			SVRC_BANNED,
			SVRC_SALT,
			SVRC_LOGGEDIN,
			SVRC_INVALIDPASSWORD,
			SVRC_MESSAGE,
			SVRC_UPDATE,

			CLRC_BEGINCONNECTION = 52,
			CLRC_PASSWORD,
			CLRC_COMMAND,
			CLRC_PONG,
			CLRC_DISCONNECT
		};

	public:
		SkulltagRConProtocol(Server *server);
		static RConProtocol	*connectToServer(Server *server, Response &response);

	public slots:
		void	disconnectFromServer();
		void	sendCommand(const QString &cmd);
		void	sendPassword(const QString &password);

	protected:
		void	run();
		void	processPacket(const char *data, int length, bool initial=false, int maxUpdates=-1, int *pos=NULL);

		QString	hostName;
		QString	salt;
		int		serverProtocolVersion;
};

#endif /* __SKULLTAGSERVER_H__ */
