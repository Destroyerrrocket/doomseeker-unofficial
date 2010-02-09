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

#include "serverapi/rconprotocol.h"
#include "serverapi/player.h"
#include "serverapi/serverstructs.h"
#include "global.h"
#include "pathfinder.h"

class TooltipGenerator;

class MAIN_EXPORT Server : public QObject
{
	Q_OBJECT

	friend class RefreshingThread;
	friend class ServerBatch;

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
		 *	Returns name of the engine for this server, for example: "Skulltag".
		 */
		virtual QString		engineName() const { return tr("Undefined"); }

		const QHostAddress	&address() const { return serverAddress; }
		const QString&		eMail() const { return email; }
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
		int					numPlayers(bool includeBots=true) const { return includeBots ? players.size() : players.size()-numBots; }
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
		 *	@param [out] error - type of error
		 */
		virtual QString		clientWorkingDirectory() const;
		virtual void		connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const;

		/**
		 *	@param [out] cli - after successful call this will contain
		 *		required command line information.
		 *	@param [out] error - if return == false, error text will be put here
		 *  @param bOfflinePlay - if true a command line for single player game
		 *		will be launched
		 *	@return	true if command line was successfully created.
		 */
		bool				createHostCommandLine(const HostInfo& hostInfo, CommandLineInfo& cli, bool bOfflinePlay, QString& error) const;

		/**
		 *	@param [out] cli - after successful call this will contain
		 *		required command line information.
		 *	@return	JoinError::type == NoError if all ok.
		 */
		JoinError			createJoinCommandLine(CommandLineInfo& cli, const QString &connectPassword) const;

		/**
		 *	@see createHostCommandLine()
		 */
		bool				host(const HostInfo& hostInfo, bool bOfflinePlay, QString& error);

		bool				isRefreshing() const { return bIsRefreshing; }

		/**
		 *	!!! DEPRECATED !!!
		 *	The proper routine is to:
		 *	-# Call createJoinCommandLine
		 *	-# Test JoinError in the GUI
		 *	-# If there are missing wads, ask if the user wants to launch the
		 *		Wadseeker. If Wadseeker is successful repeat the routine.
		 *	-# If there are no further errors, call runExecutable with
		 *		generated command line as parameter.
		 *
		 *	@return	JoinError::type == NoError if all ok.
		 */
		JoinError			join(const QString &connectPassword) const;

		/**
		 *	Returns the path to the binary for offline play.
		 *	@param [out] error - type of error
		 *	@return default behavior returns clientBinary().
		 */
		virtual QString		offlineGameBinary(QString& error) const { return clientBinary(error); }

		/**
		 *	Returns the working directory of the binary for offline game.
		 *	@param [out] error - type of error
		 *	@return Default behavior returns offlineGameBinary() directory
		 */
		virtual QString		offlineGameWorkingDirectory() const;

		/**
		 *	Executes predefined command line.
		 *	@param cli - command line that will be executed
		 *	@param bWrapWithStandardServerConsole - if true Doomseeker will
		 *		attempt to wrap the input/output of the program with it's own
		 *		console
		 *	@param [out] error - may contain error string if false is returned
		 */
		bool				runExecutable(const CommandLineInfo& cli, bool bWrapWithStandardServerConsole, QString& error) const;

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

		/**
		 *	@brief Creates an instance of TooltipGenerator.
		 *
		 *	This can be replaced by a plugin to use a custom tooltip generator.
		 *
		 *	@return Default behaviour returns default implementation of
		 *	TooltipGenerator.
		 */
		virtual TooltipGenerator*	tooltipGenerator() const;

		friend class ServerPointer;

	public slots:
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
		/**
		 *	Command line parameter that is used to set IWAD.
		 */
		virtual QString		argForIwadLoading() const { return "-iwad"; }

		/**
		 *	Command line parameter that is used to set internet port for the
		 *	game.
		 */
		virtual QString		argForPort() const { return "-port"; }

		/**
		 *	Command line parameter that is used to load a PWAD.
		 */
		virtual QString		argForPwadLoading() const { return "-file"; }

		/**
		 *	Command line parameter used to launch a server.
		 */
		virtual QString		argForServerLaunch() const { return ""; }

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

		/**
		 *	Reads response data.
		 *	@return the resposne that should be emitted. Do NOT perform any
		 *		signal emissions from within this functions. This is not thread
		 *		safe and may lead to a crash.
		 */
		virtual Response		readRequest(QByteArray &data)=0;

		/**
		 *	Prepares challenge data.
		 *	@return true on success and RESPONSE_GOOD signal should be emitted,
		 *		false otherwise.
		 */
		virtual bool		sendRequest(QByteArray &data)=0;

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

		/**
		 *	Refresher sets this to false before calling the virtual
		 *	readRequest() method. If this method sets this to true, Refresher
		 *	will not modify the currentPing field assuming that readRequest()
		 *	set currentPing to a correct value. If it remains false after the
		 *	readRequest() call Doomseeker will use a global method to determine
		 *	ping, which may be less accurate.
		 */
		bool				bPingIsSet;

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
		int					numBots; // -1 means needs to be updated
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
		 *	@return false if it's impossible to send the query (fail)
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
