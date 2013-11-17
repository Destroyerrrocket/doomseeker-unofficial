//------------------------------------------------------------------------------
// ircnetworkentity.h
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
#ifndef __IRCNETWORKENTITY_H__
#define __IRCNETWORKENTITY_H__

#include "global.h"
#include <QString>
#include <QStringList>

/**
 * @brief Data structure that describes and defines a connection to an IRC
 * network or server.
 */
class MAIN_EXPORT IRCNetworkEntity
{
	public:
		IRCNetworkEntity();

		/**
		 *	@brief Address of the server or network to connect to.
		 */
		QString				address;

		/**
		 *	@brief List of channels to which a /join command will be issued
		 *	automatically when a connection with this network is established.
		 */
		QStringList			autojoinChannels;
		/**
		 * @brief List of commands executed on network join.
		 */
		QStringList			autojoinCommands;

		/**
		 *	@brief Join this network when Doomseeker starts up.
		 */
		bool				bAutojoinNetwork;

		/**
		 *	@brief A short, human-readable description for the network.
		 *	(Preferably a single word).
		 */
		QString				description;
		QString				nickservCommand;
		QString				nickservPassword;

		/**
		 *	@brief Password for the server or network. Ignored if empty.
		 */
		QString				password;

		/**
		 *	@brief Port of the server or network to connect to.
		 *
		 *	Default value: 6667
		 */
		unsigned short		port;

		bool				isValid() const
		{
			return !address.isEmpty() && port != 0;
		}

		/**
		 *	@brief Sorts by description.
		 */
		bool				operator< (const IRCNetworkEntity& other) const
		{
			return this->description.toLower().trimmed() < other.description.toLower().trimmed();
		}

		/**
		 * @brief Checks to see if this is the same server.
		 */
		bool				operator== (const IRCNetworkEntity& other) const
		{
			return address == other.address && port == other.port;
		}
};

#endif
