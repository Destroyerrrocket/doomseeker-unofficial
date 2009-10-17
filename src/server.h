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

#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QObject>
#include <QHostAddress>
#include <QString>
#include <QStringList>
#include <QMetaType>
#include <QPixmap>
#include <QThread>
#include <QTime>
#include <QUdpSocket>

#include "global.h"
#include "pathfinder.h"

#define MAX_TEAMS 4

/**
 *	@brief Generic representation of DMFlags section.
 *
 *	Note: Maximum amount of flags in one section is 32.
 */
struct MAIN_EXPORT DMFlagsSection
{
	struct DMFlag
	{
		QString         name;
		unsigned char   value;
	};

	QString         name;
	unsigned char   size;
	DMFlag			flags[32];
};

/**
 *	List used by Server class' virtual method to return all flags sections.
 */
typedef QList<DMFlagsSection*> 							DMFlags;
typedef QList<DMFlagsSection*>::iterator				DMFlagsIt;
typedef QList<DMFlagsSection*>::const_iterator 			DMFlagsItConst;
typedef QList<const DMFlagsSection*> 					DMFlagsConst;
typedef QList<const DMFlagsSection*>::iterator			DMFlagsConstIt;
typedef QList<const DMFlagsSection*>::const_iterator 	DMFlagsConstItConst;

/**
 * Data structure that holds information about players in a server.
 */
struct MAIN_EXPORT Player
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

		/**
		 * Colorizes the given string.  Most useful for displaying colored
		 * names.
		 */
		static QString	colorizeString(const QString &str, int def=4);

	protected:
		static const char	colorChart[20][7];

		QString			playerName;
		short			currentScore;
		unsigned short	currentPing;
		bool			spectator;
		bool			bot;
		PlayerTeam		team;
};

/**
 *	@brief Struct containing info about a game console variable (like fraglimit)
 */
struct MAIN_EXPORT GameCVar
{
	/**
	 *	Nice name to display in Create Server dialog.
	 */
	QString		name;

	/**
	 *	Console command used to set the given CVar.
	 */
	QString		consoleCommand;

	GameCVar() {}
	GameCVar(QString fname, QString fconsoleCommand):name(fname),consoleCommand(fconsoleCommand) {}

	void			setValue(bool b) { b == true ? setValue("1") : setValue("0"); }
	void			setValue(int i) { setValue(QString::number(i)); }
	void			setValue(const QString& str) { val = str; }

	const QString&	value() const { return val; }
	bool			valueBool() const { static_cast<bool>(val.toInt()); }
	bool			valueInt() const { return val.toInt(); }

	protected:
		QString		val;
};

/**
 * Data structure that holds information about a servers game mode.
 */
struct MAIN_EXPORT GameMode
{
	public:
		enum StandardGameModeIndexes
		{
			SGMICooperative		= 900,
			SGMIDeathmatch		= 901,
			SGMITeamDeathmatch	= 902,
			SGMICTF				= 903
		};

		// Standard game mode set
		// These should be used in order to keep the names uniform.
		static MAIN_EXPORT const GameMode	COOPERATIVE;
		static MAIN_EXPORT const GameMode	DEATHMATCH;
		static MAIN_EXPORT const GameMode	TEAM_DEATHMATCH;
		static MAIN_EXPORT const GameMode	CAPTURE_THE_FLAG;

		/**
		 * @param name Name to display for game mode, this should be fairly short about no longer than "cooperative".
		 */
		GameMode(int index, const QString &name, bool teamgame);

		int				modeIndex() const { return gameModeIndex; }
		const QString	&name() const { return modeName;}
		bool			isTeamGame() const { return teamgame; }
	protected:
		int		gameModeIndex;
		QString	modeName;
		bool	teamgame;
};

struct MAIN_EXPORT SkillLevel
{
	//const QString strName;

	static const int	 numSkillLevels;
	static const QString names[];
};

/**
 *	This structure is used by Server Info dockable widget.
 *	Each instance of this struct is one button on the widget.
 *	The button is connected to function();
 */
struct MAIN_EXPORT ServerAction
{
	QString		label;
	QObject*	receiver;
	const char*	slot;

	ServerAction()
	{
		receiver = NULL;
		slot = NULL;
	}
};

/**
 * This structure is used by Server Info dockable widget.
 * Each instance of this struct is one label on the widget.
 */
struct MAIN_EXPORT ServerInfo
{
	QString richText;
	QString toolTip;
};

class MAIN_EXPORT Server;

/**
 *	@brief Host launch information for Server class.
 *
 *	Create Server dialog uses this to setup host information.
 *	However things that can be set through the Server class,
 *	like MOTD, max. clients, max. players, server name, etc. should
 *	be set through Server class' setters.
 */
struct MAIN_EXPORT HostInfo
{
	QString 		executablePath; /// if empty, serverBinary() will be used
	QString 		iwadPath;
	QStringList 	pwadsPaths;
	QStringList 	customParameters;
	DMFlags 		dmFlags;

	/**
	 *	Contents of this list will be passed as "+consoleCommand value"
	 *	to the command line.
	 */
	QList<GameCVar> cvars;

	~HostInfo()
	{
		foreach(DMFlagsSection* sec, dmFlags)
			delete sec;
	}
};

/**
 * An abstract interface for a remote console protocol.
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
		void			messageReceived(const QString &cmd);
		void			playerListUpdated();

	protected:
		RConProtocol(Server *server);

		bool			connected;
		QList<Player>	players;
		Server			*server;
		QUdpSocket		socket;

		friend class Server;
};

class MAIN_EXPORT Server : public QObject
{
	Q_OBJECT

	friend class RefreshingThread;

	public:
		enum Response
		{
			RESPONSE_GOOD,		// Data is available
			RESPONSE_TIMEOUT,	// Server didn't respond at all
			RESPONSE_WAIT,	 	// Server responded with "wait"
			RESPONSE_BAD,		// Probably refreshing too quickly
			RESPONSE_BANNED,	// Won't recieve data from this server ever.
			RESPONSE_NO_RESPONSE_YET	// "Dummy" response for servers that weren't refreshed yet
		};

		Server(const QHostAddress &address, unsigned short port);
		Server(const Server &other);
		virtual ~Server();

		/**
		 * Should return general info about current game (fraglimit, team scores, etc.)
		 */
		virtual QString		gameInfoTableHTML() const;
		/**
		 * Should return general info about server, like server name, version, email, etc.
		 */
		virtual QString		generalInfoHTML() const;
		/**
		 * Should return player table (the thing that is created when cursor hovers over players column)
		 */
		virtual QString		playerTableHTML() const;

		QList<ServerInfo>*	serverInfo() const;

		const QHostAddress	&address() const { return serverAddress; }
		const DMFlags		&gameFlags() const { return dmFlags; }
		const GameMode		&gameMode() const { return currentGameMode; }
		virtual bool		hasRcon() const { return false; }
		virtual const QPixmap	&icon() const=0;
		bool				isCustom() const { return custom; }
		bool				isKnown() const { return bKnown; }
		bool				isLocked() const { return locked; }
		bool				isSetToDelete() const { return bDelete; }
		const QString		&iwadName() const { return iwad; }
		int					lastResponse() const { return response; }
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
		const QStringList	&pwads() const { return wads; }
		virtual RConProtocol	*rcon() { return NULL; }
		unsigned int		score(int team=0) const { return scores[team]; }
		unsigned int		scoreLimit() const { return serverScoreLimit; }
		virtual QRgb		teamColor(int team) const;
		virtual QString		teamName(int team) const { return team < MAX_TEAMS && team >= 0 ? teamNames[team] : ""; }
		int					teamPlayerCount(int team) const;
		unsigned short		timeLeft() const { return serverTimeLeft; }
		unsigned short		timeLimit() const { return serverTimeLimit; }
		const QString		version() const { return serverVersion; }
		const QString		&wad(int index) const { return wads[index]; }
		const QString		&website() const { return webSite; }

		void				setBroadcastToLAN(bool b) { broadcastToLAN = b; }
		void				setBroadcastToMaster(bool b) { broadcastToMaster = b; }
		void				setHostEmail(const QString& mail) { email = mail; }
		void				setGameMode(const GameMode& gameMode) { currentGameMode = gameMode; }
		void				setMap(const QString& name) { mapName = name; }
		void				setMapList(const QStringList& maplist) { mapList = maplist; }
		void				setMaximumClients(unsigned short i) { maxClients = i; }
		void				setMaximumPlayers(unsigned short i) { maxPlayers = i; }
		void				setMOTD(const QString& message) { motd = message; }
		void				setName(const QString& name) { serverName = name; }
		void				setPasswordConnect(const QString& str) { passwordConnect = str; }
		void				setPasswordJoin(const QString& str) { passwordJoin = str; }
		void				setPasswordRCon(const QString& str) { passwordRCon = str; }
		void				setPort(unsigned short i) { serverPort = i; }
		void				setRandomMapRotation(bool b) { mapRandomRotation = b; }
		void				setSkill(unsigned char newSkill) { skill = newSkill; }
		void				setWebsite(const QString& site) { webSite = site; }

		void				operator= (const Server &other);
		void				setCustom(bool b) { custom = b; }
		void				setToDelete(bool b);

		QList<ServerAction>*	actions();
		virtual void			actionsEx(QList<ServerAction>*) {};

		/**
		 *	Returns the path to the client binary
		 *	@param [out] error - type of error
		 *	@return empty if error
		 */
		virtual QString		clientBinary(QString& error) const=0;

		/**
		 *	Default behavior returns directory of clientBinary(), but
		 *	you can override this to provide different working directory for
		 *	Skulltag's testing binaries.
		 */
		virtual QString		clientWorkingDirectory() const;
		virtual void		connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const;

		/**
		 *	@param [out] executablePath - path to the executable is saved here.
		 *	@param [out] applicationDir - working directory of the executable
		 *	@param [out] args - launch arguments are saved here.
		 *	@param [out] error - if return == false, error text will be put here
		 *	@return	true if command line was successfully created.
		 */
		bool				createHostCommandLine(const HostInfo& hostInfo, QFileInfo& executablePath, QDir& applicationDir, QStringList& args, QString& error) const;

		/**
		 *	@param [out] executablePath - path to the executable is saved here.
		 *	@param [out] applicationDir - working directory of the executable
		 *	@param [out] args - launch arguments are saved here.
		 *	@return	true if command line was successfully created.
		 */
		bool				createJoinCommandLine(QFileInfo& executablePath, QDir& applicationDir, QStringList& args, const QString &connectPassword) const;

		/**
		 *	@see createHostCommandLine()
		 */
		bool				host(const HostInfo& hostInfo, QString& error);

		bool				isRefreshing() const { return bIsRefreshing; }

		/**
		 *	Default behaviour returns the same string as clientBinary().
		 *	This can be reimplemented for engines that use two different
		 *	binaries for the server and for the client.
		 */
		virtual QString		serverBinary(QString& error) const { return clientBinary(error); }

		/**
		 *	Default behaviour returns directory of serverBinary().
		 */
		virtual QString		serverWorkingDirectory() const;

		friend class ServerPointer;

	public slots:

		void			displayJoinCommandLine();
		void			join(const QString &connectPassword=QString()) const;

		/**
		 * Updates the server data.
		 */
		bool			refresh();

	signals:
		void				begunRefreshing(Server* server);
		/**
		 * Emitted when a refresh has been completed.  Be sure to check the
		 * response to see if anything has actually changed.
		 * @see Response
		 */
		void				updated(Server *server, int response);

	protected:
		virtual QString		argForIwadLoading() const { return "-iwad"; }
		virtual QString		argForPort() const { return "-port"; }
		virtual QString		argForPwadLoading() const { return "-file"; }

		virtual void		additionalServerInfo(QList<ServerInfo>* baseList) const {}

		void				clearDMFlags();

		/**
		 *	On Windows this removes any wrapping " chars.
		 *
		 *	Explanation:
		 *	Draft from Qt documentation on QProcess::startDetached:
		 *	"On Windows, arguments that contain spaces are wrapped in quotes."
		 *	Thus, on Windows we must unwrap the arguments that are wrapped in
		 *	quotes because thing like +sv_hostname "Started from Doomseeker"
		 *	won't work properly and a server with empty name will be started.
		 */
		void				cleanArguments(QStringList& args) const;

		/**
		 * Wrapper function to allow refresher to emit the updated signal.
		 */
		void				emitUpdated(int response) { emit updated(this, response); }

		/**
		 *	Creates engine specific command line parameters out of passed
		 *	dmFlags list.
		 *	Default behavior does nothing.
		 */
		virtual void		hostDMFlags(QStringList& args, const DMFlags& dmFlags) const {};

		/**
		 *	Creates engine specific command line parameters out of Server class
		 *	fields.
		 *
		 *	Please note that port, and some other stuff, is already set by
		 *	createHostCommandLine().
		 *	@see createHostCommandLine() - cvars parameter.
		 */
		virtual void		hostProperties(QStringList& args) const {};

		virtual bool		readRequest(QByteArray &data)=0;
		virtual bool		sendRequest(QByteArray &data)=0;
		/**
		 *	This will return absolutely nothing if the list in the first
		 *	argument is empty.
		 */
		virtual QString		spawnPartOfPlayerTable(QList<const Player*> list, int colspan, bool bAppendEmptyRowAtBeginning) const;

		/**
		 *	If this is true server will be deleted as soon as
		 *	it finished working (refreshing). This should be safer
		 *	than blatant `delete server` while server's thread is still
		 *	running.
		 */
		bool				bDelete;
		/**
		 * This should be set to true upon successful return from doRefresh(),
		 * and to false upon failure. setServers() protected slot handles this.
		 * Example usage: Skulltag servers can use this to update ping
		 * if the server responds with "wait before refreshing".
		 */
		bool				bKnown;


		bool				broadcastToLAN;
		bool				broadcastToMaster;
		GameMode			currentGameMode;
		unsigned int		currentPing;
		bool				custom;
		DMFlags				dmFlags;
		QString				email;
		QString				iwad;
		bool				locked;
		QStringList			mapList;
		QString				mapName;
		bool				mapRandomRotation;
		unsigned short		maxClients;
		unsigned short		maxPlayers;
		QString				motd;
		QString				passwordConnect;
		QString				passwordJoin;
		QString				passwordRCon;
		QList<Player>		players;
		Response			response;
		unsigned int		scores[MAX_TEAMS];
		QString				serverName;
		unsigned int		serverScoreLimit;
		unsigned short		serverTimeLeft;
		unsigned short		serverTimeLimit;
		QString				serverVersion;
		unsigned char		skill;
		QStringList			wads;
		QString				webSite;

		static QString		teamNames[];

		QTime				time;

	protected slots:
		/**
		 * server argument here is only provided for compatibility with updated
		 * signal
		 */
		void				setResponse(Server* server, int response);

	private:
		/**
		 *	Called when server begins refreshing routine.
		 */
		void				refreshStarts();

		/**
		 *	Called when server finishes refreshing routine.
		 */
		void				refreshStops();

		/**
		 *	Method called by the refreshing thread. Sends the query
		 *	through refreshing thread socket.
		 */
		bool				sendRefreshQuery(QUdpSocket* socket);

		/**
		 * This is used to make
		 * sure that refresh() method isn't run on
		 * server that is already refreshing.
		 */
		bool				bIsRefreshing;
		QHostAddress		serverAddress;
		unsigned short		serverPort;

		int					triesLeft; /// Track how many resends we should try.
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
