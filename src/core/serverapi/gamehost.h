//------------------------------------------------------------------------------
// gamehost.h
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef idABA35264_C99B_49FA_BEDEDB4F5978C7EC
#define idABA35264_C99B_49FA_BEDEDB4F5978C7EC

#include "global.h"
#include "serverapi/polymorphism.h"
#include <QObject>
#include <QStringList>

class CommandLineInfo;
class EnginePlugin;
class GameCreateParams;
class Message;

/**
 * @ingroup group_pluginapi
 */
class MAIN_EXPORT GameHost : public QObject
{
	Q_OBJECT

	public:
		GameHost(EnginePlugin* plugin);
		virtual ~GameHost();

		/**
		 * @return Message::isError() == false if command line was
		 *         successfully created.
		 */
		Message createHostCommandLine(const GameCreateParams& params, CommandLineInfo& cmdLine);

		/**
		 * @see createHostCommandLine()
		 */
		Message host(const GameCreateParams& params);

		EnginePlugin* plugin() const;

	protected:
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
		 * @brief Command line parameter for playing back a demo.
		 *
		 * Default: "-playdemo".
		 */
		const QString& argForDemoPlayback() const;
		/**
		 * @brief Command line parameter for recording a demo.
		 *
		 * Default: "-record";
		 */
		const QString& argForDemoRecord() const;

		/**
		 * @brief Command line parameter used to launch a server.
		 *
		 * No default.
		 */
		const QString& argForServerLaunch() const;

		/**
		 * @brief Output command line arguments.
		 *
		 * This is where plugins should write all CMD line arguments they
		 * create for the executable run.
		 */
		QStringList &args();

		virtual void addCustomParameters();

		/**
		 * @brief @b [Virtual] Creates engine specific command line parameters
		 *        out of passed DM flags list.
		 *
		 * Default behavior does nothing.
		 */
		void addDMFlags();
		POLYMORPHIC_SETTER_DECLARE(void, GameHost, addDMFlags, ());
		void addDMFlags_default();

		/**
		 * @brief Creates engine specific command line parameters out of
		 *        Server class fields.
		 *
		 * Please note that port, and some other stuff, is already set by
		 * createHostCommandLine().
		 *
		 * @see createHostCommandLine() - cvars parameter.
		 */
		virtual void addExtra();

		void addIwad();
		POLYMORPHIC_SETTER_DECLARE(void, GameHost, addIwad, ());
		void addIwad_default();

		void addPwads();
		POLYMORPHIC_SETTER_DECLARE(void, GameHost, addPwads, ());
		void addPwads_default();

		virtual void createCommandLineArguments();

		const GameCreateParams& params() const;

		void setArgForIwadLoading(const QString& arg);
		void setArgForPort(const QString& arg);
		void setArgForPwadLoading(const QString& arg);
		void setArgForDemoPlayback(const QString& arg);
		void setArgForDemoRecord(const QString& arg);
		void setArgForServerLaunch(const QString& arg);

		/**
		 * @brief Call this method to convey errors.
		 *
		 * GameHost checks for errors before making certain steps. If
		 * plugins want to prevent execution of the game, they should
		 * set a Message instance that will return 'true' on Message::isError().
		 */
		void setMessage(const Message& message);

	private:
		class PrivData;
		PrivData* d;

		Q_DISABLE_COPY(GameHost);

		void addDemoPlaybackIfApplicable();
		void addDemoRecordIfApplicable();
		void saveDemoMetaData();
		void setupGamePaths();
};

#endif
