//------------------------------------------------------------------------------
// gameexefactory.h
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
#ifndef id53E6D544_EC39_49F8_93CF518BBB413164
#define id53E6D544_EC39_49F8_93CF518BBB413164

#include "dptr.h"
#include "global.h"
#include "serverapi/polymorphism.h"
#include <QObject>
#include <QList>

class EnginePlugin;
class ExeFile;
class GameFileList;
class Server;


/**
 * @ingroup group_pluginapi
 * @brief Returns executable file retrievers from plugins to Doomseeker.
 *
 * ExeFile for client executable (for server joining purposes) is returned
 * by instances of Server, as this usually needs access to the Server
 * class anyway.
 *
 * It's a caller responsibility to delete each ExeFile object that gets
 * created here.
 *
 * @see EnginePlugin::Data
 */
class MAIN_EXPORT GameExeFactory : public QObject
{
	Q_OBJECT

	public:
		GameExeFactory(EnginePlugin* plugin);
		virtual ~GameExeFactory();

		/**
		 * @brief @b [Virtual] List of all game files associated with this
		 * game.
		 *
		 * This list is used to generate path configurators in plugin
		 * configuration box.
		 *
		 * Read doc for GameFile.
		 */
		GameFileList gameFiles() const;

		/**
		 * @brief @b [Virtual] Instantiates retriever for offline game
		 *        executable.
		 *
		 * Default behavior extracts path stored in "BinaryPath" setting
		 * of plugin's config.
		 */
		ExeFile* offline();
		/**
		 * @brief Gets EnginePlugin associated with this object.
		 */
		EnginePlugin* plugin();
		/**
		 * @brief @b [Virtual] Instantiates retriever for server executable.
		 *
		 * Default behavior extracts path stored either in "BinaryPath"
		 * or "ServerBinaryPath" setting of plugin's config. This behavior
		 * depends on whether plugin declares that a separate executable
		 * should be used for hosting servers.
		 */
		ExeFile* server();

	protected:
		POLYMORPHIC_SETTER_DECLARE_CONST(GameFileList, GameExeFactory, gameFiles, ());
		GameFileList gameFiles_default() const;

		POLYMORPHIC_SETTER_DECLARE(ExeFile*, GameExeFactory, offline, ());
		ExeFile* offline_default();

		POLYMORPHIC_SETTER_DECLARE(ExeFile*, GameExeFactory, server, ());
		ExeFile* server_default();

	private:
		DPtr<GameExeFactory> d;
};

#endif
