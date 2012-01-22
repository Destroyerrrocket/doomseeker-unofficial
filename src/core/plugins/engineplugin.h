//------------------------------------------------------------------------------
// engineplugin.h
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
// Copyright (C) 2011 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __ENGINEPLUGIN_H__
#define __ENGINEPLUGIN_H__

#include <QtContainerFwd>
#include <QString>
#include <QVector>
#include "global.h"
#include "serverapi/serverstructs.h"

#define DECLARE_PLUGIN(XEnginePlugin) \
	public: \
		static EnginePlugin *staticInstance() { return &__Static_Instance; } \
	protected: \
		static XEnginePlugin __Static_Instance;

#define INSTALL_PLUGIN(XEnginePlugin) \
	XEnginePlugin XEnginePlugin::__Static_Instance; \
	extern "C" PLUGIN_EXPORT EnginePlugin *doomSeekerInit() \
	{ \
		return XEnginePlugin::staticInstance(); \
	}

class Binaries;
class ConfigurationBaseBox;
class GameCVar;
class GameMode;
class IniSection;
class IRCNetworkEntity;
class MasterClient;
class Server;
class QHostAddress;
class QPixmap;
class QWidget;

class MAIN_EXPORT EnginePlugin
{
	protected:
		/**
		* We will use this to tell Init what features we have, new features
		* can be added to this list, but the position of any feature must not
		* change.
		*/
		enum InitFeatures
		{
			EP_Done, ///< Signals the end of init parameters.

			EP_Author, ///< (const char*) Author of the plugin.
			EP_Version, ///< (unsigned int) Single version number for plugin.

			EP_AllDMFlags, ///< (const DMFlags*) Array of DMFlags objects.
			EP_AllowsConnectPassword, ///< Signifies that servers can be created with a connection password.
			EP_AllowsEmail, ///< Signifies that servers can have an administrative contact email attached.
			EP_AllowsURL, ///< Signifies that servers can provide a URL for potential wad downloads.
			EP_AllowsJoinPassword, ///< Signifies that servers can be created with a join password.
			EP_AllowsRConPassword, ///< Signifies that servers can be created for remote console access.
			EP_AllowsMOTD, ///< Signifies that servers can have a message of the day.
			EP_DefaultMaster, ///< (const char*) Default ip address and port ("address:port") for master server.  Requires EP_HasMasterServer.
			EP_DefaultServerPort, ///< (quint16) Default port for custom server creation.
			EP_GameModes, ///< (const QList&lt;GameMode&gt;*) List of all possible game modes supported.
			EP_GameModifiers, ///< (const QList&lt;GameCVar&gt;*) List of potential game modifiers.
			EP_HasMasterServer, ///< Signifies that the plugin implements a master server protocol.
			EP_InGameFileDownloads, ///< Allows the player to join a server without downloading files through Wadseeker.
			EP_IRCChannel, ///< (const char*)server, (const char*)channel - Can be repeated. Default IRC channels.
			EP_SupportsRandomMapRotation, ///< Signifies that a server can be created with a random map rotation.
			EP_RefreshThreshold ///< (quint8) The amount of time (in seconds) that must pass before a server can be requeried.
		};

		/// Reimplement if you want to perform some ini initialization manually.
		virtual void							setupConfig(IniSection &config) const {}

	public:
		/**
		 * Store the information about the plugin in a structure so that we can
		 * freely add features without invalidating existing plugins.
		 */
		class Data
		{
			public:
				/// List of all engine's DMFlags or NULL if none.
				const DMFlags*			allDMFlags;
				bool					allowsConnectPassword;
				bool					allowsEmail;
				bool					allowsJoinPassword;
				bool					allowsMOTD;
				bool					allowsRConPassword;
				bool					allowsURL;
				QString					author;
				/// Default port on which servers for given engine are hosted.
				QString					defaultMaster;
				quint16					defaultServerPort;
				/// All available game modes for the engine or NULL if none.
				const QList<GameMode>*	gameModes;
				/**
				*	@brief Returns a list of modifiers.
				*
				*	Modifiers are used and displayed in Create Server dialog.
				*	If an empty list (or NULL) is returned, Modifier combo will be
				*	disabled.
				*/
				const QList<GameCVar>*	gameModifiers;
				bool					hasMasterServer;
				/// icon of the engine
				QPixmap					*icon;
				bool					inGameFileDownloads;
				QVector<IRCNetworkEntity> ircChannels;
				QString					name;
				IniSection				*pConfig;
				quint8					refreshThreshold;
				bool					supportsRandomMapRotation;
				bool					valid;
				unsigned int			version;

				Data();
		};

		EnginePlugin();
		virtual ~EnginePlugin();

		/**
		 * Initializes a plugin based on a feature set passed in. A name for
		 * the plugin and its icon (in XPM format) must be provided. The list
		 * of features must be terminated with EP_Done.
		 *
		 * @see InitFeatures
		 * @param name Name of the plugin which will be presented to the user.
		 * @param icon XPM icon used to represent this plugin.
		 */
		void init(const char* name, const char* const icon[], ...);

		/**
		 *	@brief Engine's configuration widget.
		 */
		virtual ConfigurationBaseBox*	configuration(QWidget *parent) const;

		const Data						*data() const { return d; }
		const QPixmap					&icon() const { return *d->icon; }
		void							setConfig(IniSection &cfg) const;

		/**
		 *	@brief Returns a list of limits (like fraglimit) supported by passed
		 *	gamemode.
		 */
		virtual QList<GameCVar>			limits(const GameMode& mode) const { return QList<GameCVar>(); }

		/**
		 * Creates an MasterClient instace for this plugin.
		 */
		virtual MasterClient*			masterClient() const { return NULL; }
		/**
		 * Fills the variables with information about the master's address.
		 */
		void							masterHost(QString &host, unsigned short &port) const;

		/**
		 *	@brief Creates an instance of server object from this plugin.
		 *	This might be useful for custom servers.
		 * 	@return instance of plugin's server object
		 */
		virtual Server*					server(const QHostAddress &address, unsigned short port) const = 0;

	private:
		Data	*d;
};

#endif
