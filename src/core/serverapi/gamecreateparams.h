//------------------------------------------------------------------------------
// gamecreateparams.h
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef idF7CFB784_3591_4B2C_B5529AE7FBCC2568
#define idF7CFB784_3591_4B2C_B5529AE7FBCC2568

#include "dptr.h"
#include "global.h"
#include <QList>
#include <QString>
#include <QStringList>

class DMFlagsSection;
class GameCVar;
class GameDemo;
class GameMode;

/**
 * @ingroup group_pluginapi
 * @brief Game parametrization data used when creating new games.
 */
class MAIN_EXPORT GameCreateParams
{
	public:
		enum HostMode
		{
			Host,
			Offline,
			Demo
		};

		GameCreateParams();
		~GameCreateParams();

		/**
		 * @brief Password that allows clients to connect to the server.
		 */
		const QString& connectPassword() const;

		QStringList& customParameters();
		const QStringList& customParameters() const;
		/**
		 * @brief Contents of this list will be passed as "+consoleCommand value"
		 *        to the command line.
		 */
		QList<GameCVar>& cvars();
		const QList<GameCVar>& cvars() const;

		/**
		 * @brief Use if running in HostMode::Demo mode or recording a demo.
		 *
		 * Used either as a path to already existing demo that should
		 * be played back, or as a path to a new demo that should be
		 * recorded.
		 *
		 * When hostMode() is HostMode::Demo then this points to a
		 * demo for playback.
		 *
		 * When hostMode() is HostMode::Offline and demoRecord() is
		 * not GameDemo::NoDemo then this points to a new demo to
		 * record.
		 *
		 * @see hostMode()
		 * @see demoRecord()
		 */
		const QString& demoPath() const;
		/**
		 * @brief Type of demo to record; applicable only in Offline game.
		 *
		 * If set to record a demo, then demoPath() must also be set.
		 * Default is GameDemo::NoDemo.
		 */
		const GameDemo &demoRecord() const;
		void setDemoRecord(const GameDemo &demo);


		QList<DMFlagsSection>& dmFlags();
		const QList<DMFlagsSection>& dmFlags() const;
		const QString& email() const;
		/**
		 * @brief Path to the game executable.
		 */
		const QString& executablePath() const;
		const GameMode& gameMode() const;
		HostMode hostMode() const;

		bool isBroadcastToLan() const;
		bool isBroadcastToMaster() const;
		bool isRandomMapRotation() const;

		/**
		 * @brief Internal game password.
		 */
		const QString& ingamePassword() const;
		const QString& iwadPath() const;

		/**
		 * @brief Name of IWAD, derived from iwadPath().
		 */
		QString iwadName() const;

		/**
		 * @brief Level name as in E1M1 or MAP01.
		 */
		const QString& map() const;
		/**
		 * @brief List of maps in cycle, as in MAP01, MAP02, MAP03, and so on.
		 */
		const QStringList& mapList() const;
		int maxClients() const;
		int maxPlayers() const;
		/**
		 * @brief Derived basing on maxClients() and maxPlayers() value;
		 *        higher value wins.
		 */
		int maxTotalClientSlots() const;
		/**
		 * @brief Message of the Day.
		 */
		const QString& motd() const;
		const QString& name() const;
		unsigned short port() const;
		QList<bool>& pwadsOptional();
		const QList<bool>& pwadsOptional() const;
		QStringList& pwadsPaths();
		const QStringList& pwadsPaths() const;

		/**
		 * @brief Names of PWADs, derived from pwadsPaths().
		 */
		QStringList pwadsNames() const;

		/**
		 * @brief Password required to connect to remote admin console.
		 */
		const QString& rconPassword() const;

		void setBroadcastToLan(bool b);
		void setBroadcastToMaster(bool b);
		void setConnectPassword(const QString& pass);
		void setCustomParameters(const QStringList& customParameters);
		void setCvars(const QList<GameCVar>& cvars);
		void setDemoPath(const QString& demoPath);
		void setEmail(const QString& email);
		void setExecutablePath(const QString& executablePath);
		void setGameMode(const GameMode& mode);
		void setHostMode(HostMode mode);
		void setIngamePassword(const QString& pass);
		void setIwadPath(const QString& iwadPath);
		void setMap(const QString& map);
		void setMapList(const QStringList& mapList);
		void setMaxClients(int num);
		void setMaxPlayers(int num);
		void setMotd(const QString& motd);
		void setName(const QString& name);
		void setPort(unsigned short port);
		void setPwadsOptional(const QList<bool>& pwadsOptional);
		void setPwadsPaths(const QStringList& pwadsPaths);
		void setRandomMapRotation(bool b);
		void setRconPassword(const QString& pass);
		void setSkill(int skill);
		void setUrl(const QString& url);

		/**
		 * @brief Difficulty level.
		 */
		int skill() const;
		/**
		 * @brief URL for server's website or for WADs download.
		 */
		const QString& url() const;

	private:
		DPtr<GameCreateParams> d;
};

#endif // header
