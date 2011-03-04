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
#include <QtContainerFwd>

class IniSection;

/**
 *  If server for unknown engine is found, the engineIndex should be set
 *  to a negative value.
 */
struct CustomServerInfo
{
	QString engine;
	QString host;
	unsigned short port;
	int engineIndex;
};

/**
 *	A 'fake' master server that is used to handle
 *	custom servers from various engines.
 */
class CustomServers : public MasterClient
{
	Q_OBJECT;

	public:
		CustomServers() : MasterClient() {}

		/**
		 *	Reads data in format `(<engine_name>;<host_name>;<port>);(...)...`
		 *	and splits it to a list of CustomServerInfo objects. If a server
		 *  for unknown engine is found it will be appended anyway
		 *  but the CustomServerInfo::engineIndex will be set to a
		 *  negative value.
		 *	@param str - concatenated string in required format
		 *	@return list of custom servers
		 */
		static void						decodeConfigEntries(const QString& str, QList<CustomServerInfo>& outCustomServerInfoList);
		
		const PluginInfo*				plugin() const { return NULL; }

		/**
		 *	Convenience method - tries to read config seeking for
		 *	"CustomServers" entry, decode it and launch setServers() method.
		 *	@param receiver - object to receive server signals
		 *	@param slotUpdated - slot to receive Server::updated signals
		 *	@param slotBegunRefreshing - slot to receive Server::begunRefreshing signals
		 */
		void							readConfig(QObject* receiver, const char* slotUpdated, const char* slotBegunRefreshing);
		
		bool							readMasterResponse(QByteArray &data) { return false; }		

		/**
		 *	Since this is not required here (there's no real
		 *	master to refresh) this does nothing.
		 */
		void 							refresh() {}

		/**
		 *	Sets a list of custom servers.
		 *	@param csiList - list of custom servers. If element's
		 *		engineIndex is < 0 the element is discarded. Also
		 *		the element will be discarded if it's hostname cannot
		 *		be resolved.
		 *	@param receiver - object to receive server signals
		 *	@param slotUpdated - slot to receive Server::updated signals
		 *	@param slotBegunRefreshing - slot to receive Server::begunRefreshing signals
		 */
		void 							setServers(const QList<CustomServerInfo>& csiList, QObject* receiver, const char* slotUpdated, const char* slotBegunRefreshing);
		
	protected:
		bool							getServerListRequest(QByteArray &data) { return false; }
};

#endif
