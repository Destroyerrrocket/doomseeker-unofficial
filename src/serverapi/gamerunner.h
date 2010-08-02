//------------------------------------------------------------------------------
// gamerunner.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __GAME_RUNNER_H_
#define __GAME_RUNNER_H_

#include "serverapi/gamerunnerstructs.h"
#include "apprunner.h"
#include "global.h"
#include "pathfinder.h"
#include <QObject>
#include <QString>

struct PluginInfo;
class Server;

class MAIN_EXPORT GameRunner : public QObject
{
	public:
		GameRunner(const Server* server, IniSection *config);

		virtual void				connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword);

		/**
		 *	@param [out] cli - after successful call this will contain
		 *		required command line information.
		 *	@param [out] error - if return == false, error text will be put here
		 *  @param bOfflinePlay - if true a command line for single player game
		 *		will be launched
		 *	@return	MessageResult::isError == false if command line was
		 *		successfully created.
		 */
		MessageResult				createHostCommandLine(const HostInfo& hostInfo, CommandLineInfo& cmdLine, bool bOfflinePlay);

		/**
		 *	@param [out] cli - after successful call this will contain
		 *		required command line information.
		 *	@return	JoinError::type == NoError if all ok.
		 */
		JoinError					createJoinCommandLine(CommandLineInfo& cli, const QString &connectPassword);

		/**
		 *	@see createHostCommandLine()
		 */
		MessageResult				host(const HostInfo& hostInfo, bool bOfflinePlay);

		/**
		 *	This is supposed to return the plugin this GameRunner belongs to.
		 *	New instances of PluginInfo shouldn't be created here. Instead
		 *	each plugin should keep a global instance of PluginInfo (singleton?)
		 *	and a pointer to this instance should be returned.
		 */
		virtual const PluginInfo*	plugin() const = 0;

		/**
		 *	Executes predefined command line.
		 *	@param cli - command line that will be executed
		 *	@param bWrapWithStandardServerConsole - if true Doomseeker will
		 *		attempt to wrap the input/output of the program with it's own
		 *		console
		 */
		MessageResult				runExecutable(const CommandLineInfo& cli, bool bWrapWithStandardServerConsole);


	protected:
		/**
		 *	@brief Command line parameter that specifies the target server's IP
		 *		and port.
		 */
		virtual QString				argForConnect() const { return "-connect"; }
		
		/**
		 *	@brief Command line parameter that is used to specify connection
		 *	password.
		 *
		 *	There is no common value here so the default behavior returns a
		 *  "null" string.
		 */
		virtual QString				argForConnectPassword() const { return QString(); }		

		/**
		 *	@brief Command line parameter that is used to set IWAD.
		 */
		virtual QString				argForIwadLoading() const { return "-iwad"; }
		
		/**
		 *	@brief Command line parameter that is used to set internet port for
		 *	the	game.
		 */
		virtual QString				argForPort() const { return "-port"; }

		/**
		 *	@brief Command line parameter that is used to load a PWAD.
		 */
		virtual QString				argForPwadLoading() const { return "-file"; }

		/**
		 *	@brief Command line parameter used to launch a server.
		 */
		virtual QString				argForServerLaunch() const { return ""; }

		virtual MessageResult		hostAppendIwad();
		virtual MessageResult		hostAppendPwads();
		virtual MessageResult		hostGetBinary(bool bOfflinePlay);
		virtual MessageResult		hostGetWorkingDirectory(bool bOfflinePlay);

		/**
		 *	Creates engine specific command line parameters out of passed
		 *	dmFlags list.
		 *	Default behavior does nothing.
		 */
		virtual void				hostDMFlags(QStringList& args, const DMFlags& dmFlags) const {};

		/**
		 *	Creates engine specific command line parameters out of Server class
		 *	fields.
		 *
		 *	Please note that port, and some other stuff, is already set by
		 *	createHostCommandLine().
		 *	@see createHostCommandLine() - cvars parameter.
		 */
		virtual void				hostProperties(QStringList& args) const {};

		CommandLineInfo*			currentCmdLine;
		const HostInfo*				currentHostInfo;
		const Server*				server;

		IniSection					*config;
};

#endif
