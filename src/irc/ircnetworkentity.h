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

#include <QString>
	
class IRCNetworkEntity
{
	public:
		IRCNetworkEntity()
		{
			this->port = 6667;
		}
	
		/**
		 *	@brief Address of the server or network to connect to.
		 */
		QString				address;
		
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
		
		/**
		 *	@brief Sorts by description.
		 */
		bool				operator< (const IRCNetworkEntity& other) const
		{
			return this->description.toLower().trimmed() < other.description.toLower().trimmed();
		}
		
};

#endif
