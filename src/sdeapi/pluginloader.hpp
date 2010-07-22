// Emacs style mode select   -*- C++ -*-
// =============================================================================
// ### ### ##   ## ###  #   ###  ##   #   #  ##   ## ### ##  ### ###  #  ###
// #    #  # # # # #  # #   #    # # # # # # # # # # #   # #  #   #  # # #  #
// ###  #  #  #  # ###  #   ##   # # # # # # #  #  # ##  # #  #   #  # # ###
//   #  #  #     # #    #   #    # # # # # # #     # #   # #  #   #  # # #  #
// ### ### #     # #    ### ###  ##   #   #  #     # ### ##  ###  #   #  #  #
//                                     --= http://bitowl.com/sde/ =--
// =============================================================================
// Copyright (C) 2008 "Blzut3" (admin@maniacsvault.net)
// The SDE Logo is a trademark of GhostlyDeath (ghostlydeath@gmail.com)
// =============================================================================
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
// =============================================================================
// Description:
// =============================================================================

#ifndef __PLUGINLOADER_HPP__
#define __PLUGINLOADER_HPP__

#include <QObject>
#include <QString>
#include <QList>

#include "global.h"

#ifdef Q_OS_WIN32
#include <windows.h>

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
#include "masterclient.h"
#include "gui/configBase.h"

class Binaries;

class MAIN_EXPORT EnginePlugin
{
	public:
		/**
		 *	@brief List of all engine's DMFlags or NULL if none.
		 */
		virtual const DMFlags*					allDMFlags() const = 0;

		virtual bool							allowsURL() const = 0;
		virtual bool							allowsEmail() const = 0;
		virtual bool							allowsConnectPassword() const = 0;
		virtual bool							allowsJoinPassword() const = 0;
		virtual bool							allowsRConPassword() const = 0;
		virtual bool							allowsMOTD() const = 0;
		
		/**
		 *	@brief Engine's configuration widget.
		 */
		virtual ConfigurationBoxInfo*			configuration(Config *cfg, QWidget *parent) const=0;

		/**
		 *	@brief Default port on which servers for given engine are hosted.
		 */
		virtual unsigned short					defaultServerPort() const = 0;

		/**
		*	@brief All available game modes for the engine or NULL if none.
		*/
		virtual const QList<GameMode>*			gameModes() const = 0;

		/**
		 *	@brief Returns a list of modifiers.
		 *
		 *	Modifiers are used and displayed in Create Server dialog.
		 *	If an empty list (or NULL) is returned, Modifier combo will be
		 *	disabled.
		 */
		virtual const QList<GameCVar>*			gameModifiers() const = 0;

		/**
		 *	@brief False for plugins which have no master.
		 */
		virtual bool							hasMasterServer() const = 0;

		/**
		 *	@brief Returns a list of limits (like fraglimit) supported by passed
		 *	gamemode.
		 */
		virtual QList<GameCVar>					limits(const GameMode& mode) const = 0;

		/**
		 *	@return icon of the engine
		 */
		virtual QPixmap							icon() const=0;

		virtual MasterClient*					masterClient() const=0;
		/**
		 * Fills the variables with information about the master's address.
		 */
		virtual void							masterHost(QString &host, unsigned short &port) const=0;

		/**
		 *	@brief Creates an instance of server object from this plugin.
		 *	This might be useful for custom servers.
		 * 	@return instance of plugin's server object
		 */
		virtual Server*							server(const QHostAddress &address, unsigned short port) const=0;

		virtual bool							supportsRandomMapRotation() const = 0;
};
////////////////////////////////////////////////////////////////////////////////

#define MAKEID(a,b,c,d) (quint32(a)|(quint32(b)<<8)|(quint32(c)<<16)|(quint32(d)<<24))
/**
 * This is a struct to store information about a specific plugin.  This should
 * returned by the plugins themselves.
 */
struct MAIN_EXPORT PluginInfo
{
	public:

		const char*			name;
		const char*			description;
		const char*			author;
		quint8				version[4];
		quint32				type; ///< Use MAKEID to generate a check type.
		const EnginePlugin	*pInterface;
};

/**
 * This class handles one specific plugin.  It allows for cross-platform access
 * to the plugins.
 */
class MAIN_EXPORT Plugin
{
	public:
		/**
		 * Inits a plugin.  Type is an id which it compares with any possible
		 * plugins to confirm it is the right type.
		 */
		Plugin(unsigned int type, QString file);
		~Plugin();

		/**
		 * Returns a pointer to the requested function or NULL.
		 */
		void	*function(const char* func) const;

		void	initConfig();

		bool	isValid() const { return library != NULL; }

		const PluginInfo	*info;

	private:
		void	unload();

		QString	file;

#ifdef Q_OS_WIN32
		HMODULE		library;
#else
		void		*library;
#endif
};

class MAIN_EXPORT PluginLoader
{
	public:
		/**
		 * Gathers information about plugins in a particular directory.
		 * @param directoryLength length of the directory argument.  You do not need to supply if directory is NULL terminated.
		 */
		PluginLoader(unsigned int type, const QStringList &baseDirectories, const char* directory, int directoryLength=-1);
		~PluginLoader();

		/**
		 * Clears the plugins list
		 */
		void clearPlugins();

		/**
		 * Inits configuration for plugins.
		 */
		void initConfig();

		/**
		 * Gets the number of loaded plugins.  It will return 0 in safe mode.
		 */
		const unsigned int numPlugins() const;

		/**
		 *	Looks for a plugin which info::name equals to parameter.
		 * 	@param name - name to look for.
		 *	@return index of found plugin in the plugin array, or -1
		 *		if not found.
		 */
		int pluginIndexFromName(const QString& name) const;

		/**
		 * Resets the plugins directory, clearing the loaded plugins and getting new loaded plugins in the process.
		 */
		void resetPluginsDirectory(const QString& pluginsDirectory);

		/**
		 * Returns the requested plugin or NULL.
		 */
		const Plugin* operator[] (unsigned int index) const;

	private:
		bool	filesInDir();

		unsigned int		type;
		QString				pluginsDirectory;
		QList<Plugin *>		pluginsList;
};

#endif /* __PLUGINLOADER_HPP__ */
