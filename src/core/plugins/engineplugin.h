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
			EP_Done,

			EP_Author, // (const char*)
			EP_Version, // (unsigned int)

			EP_AllDMFlags, // (const DMFlags*)
			EP_AllowsConnectPassword,
			EP_AllowsEmail,
			EP_AllowsURL,
			EP_AllowsJoinPassword,
			EP_AllowsRConPassword,
			EP_AllowsMOTD,
			EP_DefaultMaster, // (const char*)
			EP_DefaultServerPort, // (quint16)
			EP_GameModes, // (const QList<GameMode>*)
			EP_GameModifiers, // (const QList<GameCVar>*)
			EP_HasMasterServer,
			EP_IRCChannel, // (const char*)server, (const char*)channel - Can be repeated
			EP_SupportsRandomMapRotation
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
				QVector<IRCNetworkEntity> ircChannels;
				QString					name;
				IniSection				*pConfig;
				bool					supportsRandomMapRotation;
				bool					valid;
				unsigned int			version;

				Data();
		};

		EnginePlugin();
		virtual ~EnginePlugin();

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
