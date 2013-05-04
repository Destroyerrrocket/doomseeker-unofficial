//------------------------------------------------------------------------------
// respondercfg.h
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
#ifndef PLUGIN_RESPONDER_RESPONDERCFG_H
#define PLUGIN_RESPONDER_RESPONDERCFG_H

class ResponderCfg
{
	public:
		/**
		 * @brief UDP port on which the master server will be hosted.
		 *
		 * If you change this, it's probable that you will also have
		 * to change master server port in Doomseeker configuration.
		 */
		static unsigned short masterServerPort();
		/**
		 * @brief Max packet size of response packets.
		 *
		 * This is used by master server. This value can be lowered
		 * to test if Doomseeker receives segmented responses properly.
		 */
		static unsigned short maxPacketSize();
		/**
		 * @brief Base UDP port at which fake game servers will be
		 *       created.
		 *
		 * Each fake server opens a new port. Ports start at this
		 * value and are gradually incremented until all servers
		 * are started. If plugin fails to bind a port, the game server
		 * will be missing from the master server list, and a proper
		 * log message will be printed.
		 *
		 * @see numServers()
		 */
		static unsigned short serverPortBase();
		/**
		 * @brief Number of fake game servers.
		 *
		 * This number is limited by how many UDP sockets can be bound
		 * on underlying Operating System.
		 *
		 * @see maxPacketSize()
		 */
		static unsigned short numServers();
};

#endif
