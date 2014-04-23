//------------------------------------------------------------------------------
// vavoomengineplugin.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "plugins/engineplugin.h"

#include "vavoomgameinfo.h"
#include "vavoomengineplugin.h"
#include "vavoommasterclient.h"
#include "vavoomserver.h"

INSTALL_PLUGIN(VavoomEnginePlugin)

VavoomEnginePlugin::VavoomEnginePlugin()
{
	const // clear warnings
	#include "vavoom.xpm"

	init("Vavoom", vavoom_xpm,
		EP_Author, "The Doomseeker Team",
		EP_Version, 7,

		EP_AllowsURL,
		EP_AllowsEmail,
		EP_AllowsConnectPassword,
		EP_AllowsJoinPassword,
		EP_AllowsRConPassword,
		EP_AllowsMOTD,
		EP_DefaultServerPort, 26000,
		EP_HasMasterServer,
		EP_DefaultMaster, "altdeath.com:26001",
		EP_GameModes, VavoomGameInfo::gameModes(),
		EP_Done
	);
}

MasterClient *VavoomEnginePlugin::masterClient() const
{
	return new VavoomMasterClient();
}

ServerPtr VavoomEnginePlugin::mkServer(const QHostAddress &address, unsigned short port) const
{
	return ServerPtr(new VavoomServer(address, port));
}
