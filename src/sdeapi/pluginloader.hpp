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

#ifdef Q_WS_WIN
#include <windows.h>

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
#include "masterclient.h"
#include "gui/configBase.h"

class MAIN_EXPORT EnginePlugin
{
	public:
		virtual ConfigurationBoxInfo	*configuration(Config *cfg, QWidget *parent) const=0;
		virtual QPixmap					icon() const=0;
		virtual MasterClient			*masterClient() const=0;
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

#ifdef Q_WS_WIN
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
		PluginLoader(unsigned int type, const char* directory, int directoryLength=-1);
		~PluginLoader();

		/**
		 * Inits configuration for plugins.
		 */
		void initConfig();

		/**
		 * Gets the number of loaded plugins.  It will return 0 in safe mode.
		 */
		const unsigned int numPlugins() const;
		/**
		 * Returns the requested plugin or NULL.
		 */
		const Plugin* operator[] (unsigned int index) const;

	private:
		void	filesInDir();

		unsigned int		type;
		QString				pluginsDirectory;
		QList<Plugin *>		pluginsList;
};

#endif /* __PLUGINLOADER_HPP__ */
