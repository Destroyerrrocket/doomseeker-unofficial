//------------------------------------------------------------------------------
// gameclientrunner.h
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

// TODO: Unvirtualize everything here! Use method pointers!

#ifndef id50da6ce1_f633_485e_9e5f_e808731b1e2e
#define id50da6ce1_f633_485e_9e5f_e808731b1e2e

#include "pathfinder/pathfinder.h"
#include "serverapi/gamerunnerstructs.h"
#include "apprunner.h"
#include "global.h"
#include <QObject>
#include <QString>

class Server;

/**
 * @brief A DTO for GameClientRunner; exchanges information between main program
 *        and plugins, and allows future extensions.
 *
 * This object may be copied freely.
 */
class MAIN_EXPORT ServerConnectParams
{
	public:
		ServerConnectParams();
		ServerConnectParams(const ServerConnectParams& other);
		ServerConnectParams& operator=(const ServerConnectParams& other);
		virtual ~ServerConnectParams();

		/**
		 * @brief Password for server connection.
		 */
		const QString& connectPassword() const;
		const QString& demoName() const;

		void setConnectPassword(const QString& val);
		void setDemoName(const QString& val);

	private:
		class PrivData;
		PrivData* d;
};

class MAIN_EXPORT GameClientRunner : public QObject
{
	public:
		GameClientRunner(Server* server);
		virtual ~GameClientRunner();

		/**
		 * @param [out] cli
		 *     After successful call this will contain
		 *     required command line information.
		 * @param params
		 *     Connection parameters specified through Doomseeker.
		 * @return JoinError::type == NoError if all ok.
		 */
		JoinError createJoinCommandLine(CommandLineInfo &cli,
			const ServerConnectParams &params);

	protected:
		void addConnectCommand();
		void addCustomParameters();
		void addDemoRecordCommand();

		/**
		 * @brief Plugins can easily add plugin-specific arguments here.
		 *
		 * This method is called at the end of "add stuff" chain.
		 */
		virtual void addExtra() {};

		void addGamePaths();
		virtual void addIwad();
		void addWads();
		void addPwads();
		void addPassword();

		/**
		 * @brief Output command line arguments.
		 *
		 * This is where plugins should write all CMD line arguments they
		 * create for the executable run.
		 */
		QStringList& args();

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

		/**
		 * @return false to terminate the join process.
		 */
		virtual bool connectParameters();
		virtual void createCommandLineArguments();

		/**
		 * @brief Password for server connection.
		 */
		const QString& connectPassword() const;
		const QString& demoName() const;

		bool isIwadFound() const;
		const QString& iwadPath() const;

		void markPwadAsMissing(const QString& pwadName);

		/**
		 * @brief Reference to a PathFinder belonging to this GameClientRunner.
		 *
		 * Useful if plugins want to access the PathFinder.
		 *
		 * This PathFinder is set up by a call to setupPathFinder(), which can
		 * be overwritten by plugis.
		 */
		PathFinder& pathFinder();

		ServerConnectParams& serverConnectParams();
		virtual void setupPathFinder();

		void setArgForConnect(const QString& arg);
		void setArgForConnectPassword(const QString& arg);
		void setArgForIwadLoading(const QString& arg);
		void setArgForPort(const QString& arg);
		void setArgForPwadLoading(const QString& arg);
		void setArgForDemoRecord(const QString& arg);

		void setJoinError(const JoinError& e);

		/**
		 * @brief Directory where Doomseeker stores downloaded WADs.
		 *
		 * This can be useful for games that support in-game downloads.
		 */
		QString wadTargetDirectory() const;

	private:
		class GamePaths
		{
			public:
				QString clientExe;
				QString offlineExe;
				QString workingDir;

				bool isValid() const
				{
					return !clientExe.isEmpty();
				}
		};

		class PrivData;

		PrivData* d;

		QString findIwad() const;
		GamePaths gamePaths();
		const QString& pluginName() const;
		void saveDemoMetaData();
};

#endif
