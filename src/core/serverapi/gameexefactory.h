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

#include "global.h"
#include <QObject>

class EnginePlugin;
class ExeFile;
class Server;

/**
 * @brief A builder strategy for GameExeFactory.
 *
 * Inherit from this class and replace the get() method. The main program
 * should never modify this class definition unless it's desired to break
 * the ABI.
 */
class MAIN_EXPORT ExeRetriever
{
	public:
		virtual ExeFile* get() = 0;
};

/**
 * @brief Returns executable file retrievers from plugins to Doomseeker.
 *
 * This class has been designed so that it can be extended freely without
 * risk of breaking the ABI. ABI may be broken if some methods are found as
 * obsolete. These methods should be marked as deprecated, but still supported.
 * In the future they will eventually be removed.
 *
 * There is no need for inheritance in order to customize the behavior of
 * this class. Instead, find an instance created by the main program
 * and set appropriate strategies for necessary types of executables.
 *
 * Remember the following principles:
 * - If a strategy is set, it is the caller's responsibility to @b delete it.
 * - If no custom strategy is provided, the class will use a default one.
 * - Most plugins won't have to set any strategies as the default are
 *   sufficient. Common usage would be to support alternate builds depending
 *   on server parameters.
 * - Refer to setter methods for more detailed info on behavior of each
 *   strategy.
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
		/**
		 * @brief Virtual plugin is provided in case if there's any reason
		 *        to inherit from this class and replace the original instance.
		 */
		virtual ~GameExeFactory();

		/**
		 * @brief Instantiates retriever for offline game executable.
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
		 * @brief Instantiates retriever for server executable.
		 *
		 * Default behavior extracts path stored either in "BinaryPath" 
		 * or "ServerBinaryPath" setting of plugin's config. This behavior
		 * depends on whether plugin declares that a separate executable
		 * should be used for hosting servers.
		 */
		ExeFile* server();

		/**
		 * @brief Sets custom strategy for offline game executable retriever
		 *        generation.
		 */
		void setOfflineExeRetriever(ExeRetriever* retriever);
		/**
		 * @brief Sets custom strategy for server executable retriever
		 *        generation.
		 */
		void setServerExeRetriever(ExeRetriever* retriever);

	private:
		class PrivData;
		PrivData* d;
};

#endif
