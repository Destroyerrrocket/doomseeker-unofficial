//------------------------------------------------------------------------------
// customservers.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __CUSTOMSERVERS_H_
#define __CUSTOMSERVERS_H_

#include "masterclient.h"

class CustomServers : public MasterClient
{
	Q_OBJECT;

	public:
		/**
		 *	Adds a custom server to the list.
		 *	@param address - instance of a server, from now on CustomServers class takes
		 *		ownership of this server. Deleting it is not required.
		 */
		void addServer(Server*);
};

#endif
