//------------------------------------------------------------------------------
// zdaemonmain.cpp
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "plugins/engineplugin.h"

#include "zdaemongameinfo.h"
#include "zdaemonmain.h"
#include "zdaemonmasterclient.h"
#include "zdaemonserver.h"

EnginePlugin* ZDaemonMain::info;

class ZDaemonEnginePlugin : public EnginePlugin
{
	public:
		ZDaemonEnginePlugin()
		{
			ZDaemonMain::info = this;

			const // clear warnings
			#include "zdaemon.xpm"

			init("ZDaemon", zdaemon_xpm,
				EP_Author, "The Doomseeker Team",
				EP_Version, 3,

				EP_AllDMFlags, ZDaemonGameInfo::dmFlags(),
				EP_AllowsURL,
				EP_AllowsEmail,
				EP_AllowsConnectPassword,
				EP_AllowsJoinPassword,
				EP_AllowsRConPassword,
				EP_AllowsMOTD,
				EP_DefaultServerPort, 10666,
				EP_HasMasterServer,
				EP_DefaultMaster, "master.zdaemon.org:15300",
				EP_Done
			);
		}

		MasterClient *masterClient() const
		{
			return new ZDaemonMasterClient();
		}

		Server* server(const QHostAddress &address, unsigned short port) const
		{
			return new ZDaemonServer(address, port);
		}
};
INSTALL_PLUGIN(ZDaemonEnginePlugin)
