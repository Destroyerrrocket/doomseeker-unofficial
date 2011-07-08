//------------------------------------------------------------------------------
// chocolatedoomengineplugin.cpp
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

#include "chocolatedoomengineplugin.h"
#include "chocolatedoommasterclient.h"
#include "chocolatedoomserver.h"

INSTALL_PLUGIN(ChocolateDoomEnginePlugin)

ChocolateDoomEnginePlugin::ChocolateDoomEnginePlugin()
{
	const // clear warnings
	#include "chocolatedoom.xpm"

	init("Chocolate Doom", chocolatedoom_xpm,
		EP_Author, "The Doomseeker Team",
		EP_Version, 4,

		EP_DefaultMaster, "master.chocolate-doom.org:2342",
		EP_DefaultServerPort, 2342,
		EP_HasMasterServer,
		EP_IRCChannel, "Chocolate Doom", "irc.oftc.net", "#chocolate-doom",
		EP_Done
	);
}

MasterClient *ChocolateDoomEnginePlugin::masterClient() const
{
	return new ChocolateDoomMasterClient();
}

Server* ChocolateDoomEnginePlugin::server(const QHostAddress &address, unsigned short port) const
{
	return new ChocolateDoomServer(address, port);
}
