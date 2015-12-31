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
#ifndef id50da6ce1_f633_485e_9e5f_e808731b1e2e
#define id50da6ce1_f633_485e_9e5f_e808731b1e2e

#include "pathfinder/pathfinder.h"
#include "serverapi/joinerror.h"
#include "serverapi/polymorphism.h"
#include "serverapi/serverptr.h"
#include "dptr.h"
#include "global.h"
#include <QObject>
#include <QString>

class CommandLineInfo;
class Message;
class Server;

/**
 * @ingroup group_pluginapi
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
		/**
		 * @brief Name of demo if demo is to be recorded.
		 *
		 * If no demo shall be recorded this will be empty.
		 */
		const QString& demoName() const;
		/**
		 * @brief In-game "join" password.
		 */
		const QString& inGamePassword() const;

		void setConnectPassword(const QString& val);
		void setDemoName(const QString& val);
		void setInGamePassword(const QString& val);

	private:
		DPtr<ServerConnectParams> d;
};

/**
 * @ingroup group_pluginapi
 * @brief Creates command line that launches the client executable of the game
 *        and connects it to a server.
 *
 * Basing on some input parameters object of this class fills out
 * CommandLineInfo objects that can be used to launch games and connect them
 * to game servers. GameClientRunner is connected to a Server object
 * from which it originates and requires for that object to be valid.
 *
 * Method of command line arguments generation can be customized either
 * partially by replacing one of the virtual methods or fully by replacing
 * createCommandLineArguments() method. In latter case it's entirely up to
 * the plugin to fill out everything either by calling already defined
 * protected methods or by filling out the CommandLineInfo struct on its own.
 */
class MAIN_EXPORT GameClientRunner : public QObject
{
	Q_OBJECT

	public:
		GameClientRunner(ServerPtr server);
		virtual ~GameClientRunner();

		/**
		 * @brief Fills out CommandLineInfo object that allows client
		 *        executables to be launched.
		 *
		 * @param [out] cli
		 *     After successful call this will contain
		 *     required command line information.
		 * @param params
		 *     Connection parameters specified through Doomseeker.
		 * @return JoinError::type == NoError if all ok.
		 */
		JoinError createJoinCommandLine(CommandLineInfo &cli,
			const ServerConnectParams &params);

		/**
		 * @brief JoinError set by last call to createJoinCommandLine().
		 */
		JoinError joinError() const;

	protected:
		/**
		 * @brief @b [Virtual] Adds connection arguments to the list.
		 *
		 * This is argForConnect() followed by server "ip:port".
		 */
		void addConnectCommand();
		POLYMORPHIC_SETTER_DECLARE(void, GameClientRunner, addConnectCommand, ());
		void addConnectCommand_default();

		/**
		 * @brief Adds custom parameters defined by user in configuration box
		 *        to the args list.
		 *
		 * Custom parameters are specified as a single string by the user,
		 * but they're split to separate arguments in a manner appropriate
		 * to given OS.
		 */
		void addCustomParameters();
		/**
		 * @brief Adds command for demo recording.
		 *
		 * This is argForDemoRecord() followed by demoName().
		 *
		 * @warning
		 *     This method doesn't check whether the demo name is empty
		 *     and attempts to add the arguments anyway.
		 */
		void addDemoRecordCommand();

		/**
		 * @brief @b [Virtual] Plugins can easily add plugin-specific arguments
		 *        here.
		 *
		 * This method is called at the end of "add stuff" chain in
		 * createCommandLineArguments().
		 */
		void addExtra();
		POLYMORPHIC_SETTER_DECLARE(void, GameClientRunner, addExtra, ());
		void addExtra_default();

		/**
		 * @brief @b [Virtual] Sets working directory and path to executable in
		 *        out put CommandLineInfo.
		 */
		void addGamePaths();
		POLYMORPHIC_SETTER_DECLARE(void, GameClientRunner, addGamePaths, ());
		void addGamePaths_default();
		/**
		 * @brief @b [Virtual] Adds in-game password to the args list.
		 *
		 * Adds argForInGamePassword() followed by inGamePassword().
		 *
		 * @warning
		 *     This method doesn't check whether the password is empty
		 *     and attempts to add the arguments anyway.
		 */
		void addInGamePassword();
		POLYMORPHIC_SETTER_DECLARE(void, GameClientRunner, addInGamePassword, ());
		void addInGamePassword_default();
		/**
		 * @brief @b [Virtual] Plugins can replace IWAD discovery mechanism
		 *        and generation of relevant executable parameters here.
		 *
		 * This method supports WAD aliasing configured in Doomseeker.
		 */
		void addIwad();
		POLYMORPHIC_SETTER_DECLARE(void, GameClientRunner, addIwad, ());
		void addIwad_default();

		/**
		 * @brief Calls addIwad() then addPwads(), sets JoinError::MissingWads
		 *        in case of failure.
		 */
		void addWads();
		/**
		 * @brief Finds and adds each PWAD to the args list, marks missing WADs.
		 *
		 * Properly found WADs are added to the args list. Each WAD argument is
		 * prepended with argForPwadLoading() argument.
		 *
		 * Not found WADs are marked as such with markPwadAsMissing() method.
		 *
		 * This method supports WAD aliasing configured in Doomseeker.
		 */
		void addPwads();
		/**
		 * @brief @b [Virtual] Adds connect password to the args list.
		 *
		 * This is argForConnectPassword() followed by connectPassword().
		 *
		 * @warning
		 *     This method doesn't check whether the password is empty
		 *     and attempts to add the arguments anyway.
		 */
		void addPassword();
		POLYMORPHIC_SETTER_DECLARE(void, GameClientRunner, addPassword, ());
		void addPassword_default();

		/**
		 * @brief Output command line arguments.
		 *
		 * This is where plugins should write all CMD line arguments they
		 * create for the executable run.
		 */
		QStringList& args();

		const QString& argForBexLoading() const;

		/**
		 * @brief Command line parameter that specifies the target server's IP
		 *        and port.
		 *
		 * Default: "-connect".
		 */
		const QString& argForConnect() const;

		/**
		 * @brief Command line parameter that is used to specify connection
		 *        password.
		 *
		 * There is no common value here so the default behavior returns a
		 * "null" string.
		 */
		const QString& argForConnectPassword() const;

		/**
		 * @brief Command line parameter that is used to load a DEHACKED file.
		 *
		 * Default: "-deh".
		 */
		const QString& argForDehLoading() const;

		/**
		 * @brief Command line parameter that is used to specify in-game
		 *        ("join") password.
		 *
		 * There is no common value here so the default behavior returns a
		 * "null" string.
		 */
		const QString& argForInGamePassword() const;

		/**
		 * @brief Command line parameter that is used to set IWAD.
		 *
		 * Default: "-iwad".
		 */
		const QString& argForIwadLoading() const;

		/**
		 * @brief Command line parameter that is used to set internet port for
		 *        the game.
		 *
		 * Default: "-port".
		 */
		const QString& argForPort() const;

		/**
		 * @brief Command line parameter that is used to load optional WADs.
		 *
		 * Default: "-file".
		 */
		const QString& argForOptionalWadLoading() const;

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
		 * @brief @b [Virtual] Spawns entire command line for client executable
		 *        launch.
		 *
		 * Default behavior splits the call between various "add*" methods.
		 * Plugins can customize behavior by either overwriting these "add*"
		 * methods that are virtual, or by overwriting this method and either
		 * writing the generation process from scratch or reusing the
		 * "add*" methods where applicable.
		 */
		void createCommandLineArguments();
		POLYMORPHIC_SETTER_DECLARE(void, GameClientRunner, createCommandLineArguments, ());
		void createCommandLineArguments_default();

		/**
		 * @brief Password for server connection.
		 */
		const QString& connectPassword() const;
		/**
		 * @brief Name of the demo if demo should be recorded, otherwise empty.
		 */
		const QString& demoName() const;

		/**
		 * @brief Finds WAD in a way that supports user configured aliases.
		 */
		QString findWad(const QString &wad) const;

		/**
		 * @brief "Join" password required in game.
		 */
		const QString& inGamePassword() const;

		bool isIwadFound() const;
		const QString& iwadPath() const;

		/**
		 * @brief Stores PWAD in an internal list of missing WADs.
		 *
		 * This information is retrieved by Doomseeker to prompt user whether
		 * they want to try to download the WADs.
		 */
		void markPwadAsMissing(const PWad& pwadName);

		/**
		 * @brief Reference to a PathFinder belonging to this GameClientRunner.
		 *
		 * Useful if plugins want to access the PathFinder.
		 */
		PathFinder& pathFinder();

		/**
		 * @brief Direct access to ServerConnectParams associated with
		 *        current command line generation.
		 */
		ServerConnectParams& serverConnectParams();

		void setArgForBexLoading(const QString& arg);
		void setArgForConnect(const QString& arg);
		void setArgForConnectPassword(const QString& arg);
		void setArgForDehLoading(const QString& arg);
		void setArgForDemoRecord(const QString& arg);
		void setArgForInGamePassword(const QString& arg);
		void setArgForIwadLoading(const QString& arg);
		void setArgForOptionalWadLoading(const QString& arg);
		void setArgForPort(const QString& arg);
		void setArgForPwadLoading(const QString& arg);

		void setExecutable(const QString &path);
		void setWorkingDir(const QString &path);

		/**
		 * @brief Apply error that is passed to the launching routine and
		 *        can be displayed to user.
		 */
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
				QString workingDir;

				bool isValid() const
				{
					return !clientExe.isEmpty();
				}
		};

		DPtr<GameClientRunner> d;

		bool canDownloadWadsInGame() const;
		bool isFatalError() const;
		QString findIwad() const;
		GamePaths gamePaths();
		const QString& pluginName() const;
		void saveDemoMetaData();

		/**
		 * @brief Sets up PathFinder that is used to search for
		 *        WADs, or other files if necessary.
		 *
		 * This is done by calling Server::wadPathFinder().
		 */
		void setupPathFinder();
};

#endif
