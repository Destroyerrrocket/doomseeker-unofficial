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

class Server;

class MAIN_EXPORT GameRunner : public QObject
{
	public:
		GameRunner(const Server* server);
		virtual ~GameRunner();

		/**
		 * @return false to terminate the join process.
		 */
		virtual bool connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword, const QString &wadTargetDirectory);

		/**
		 * @param [out] cli - after successful call this will contain
		 *     required command line information.
		 * @param managedDemo - Set to true if we should record to the demo
		 *     directory and store meta data.
		 * @return JoinError::type == NoError if all ok.
		 */
		JoinError createJoinCommandLine(CommandLineInfo& cli, const QString &connectPassword, bool managedDemo);

		/**
		 * @brief Executes predefined command line.
		 *
		 * @param cli - command line that will be executed
		 * @param bWrapWithStandardServerConsole - if true Doomseeker will
		 *     attempt to wrap the input/output of the program with it's own
		 *     console
		 */
		Message runExecutable(const CommandLineInfo& cli, bool bWrapWithStandardServerConsole);


	protected:
		/**
		 * @brief Command line parameter that specifies the target server's IP
		 * and port.
		 *
		 * Default: "-connect".
		 */
		const QString& argForConnect() const;

		/**
		 * @brief Command line parameter that is used to specify connection
		 * password.
		 *
		 * There is no common value here so the default behavior returns a
		 * "null" string.
		 */
		const QString& argForConnectPassword() const;

		/**
		 * @brief Command line parameter that is used to set IWAD.
		 *
		 * Default: "-iwad".
		 */
		const QString& argForIwadLoading() const;

		/**
		 * @brief Command line parameter that is used to set internet port for
		 * the game.
		 *
		 * Default: "-port".
		 */
		const QString& argForPort() const;

		/**
		 * @brief Command line parameter that is used to load a PWAD.
		 *
		 * Default: "-file".
		 */
		const QString& argForPwadLoading() const;

		/**
		 * @brief Command line parameter for recording a demo.
		 *
		 * Default: "-record";
		 */
		const QString& argForDemoRecord() const;

		void setArgForConnect(const QString& arg);
		void setArgForConnectPassword(const QString& arg);
		void setArgForIwadLoading(const QString& arg);
		void setArgForPort(const QString& arg);
		void setArgForPwadLoading(const QString& arg);
		void setArgForDemoRecord(const QString& arg);

	private:
		class PrivData;

		PrivData* d;
};

#endif
