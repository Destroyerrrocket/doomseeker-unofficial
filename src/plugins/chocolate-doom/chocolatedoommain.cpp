//------------------------------------------------------------------------------
// chocolatedoommain.cpp
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

#include "main.h"
#include "strings.h"
#include "irc/entities/ircnetworkentity.h"
#include "plugins/engineplugin.h"

#include "chocolatedoommain.h"
#include "chocolatedoommasterclient.h"
#include "chocolatedoomserver.h"

EnginePlugin *ChocolateDoomMain::chocolatedoom_info;

class ChocolateDoomEnginePlugin : public EnginePlugin
{
	public:
		ChocolateDoomEnginePlugin()
		{
			ChocolateDoomMain::chocolatedoom_info = this;

			const // clear warnings
			#include "chocolatedoom.xpm"

			init("Chocolate Doom", chocolatedoom_xpm,
				EP_Author, "The Doomseeker Team",
				EP_Version, 4,

				EP_DefaultMaster, "master.chocolate-doom.org:2342",
				EP_DefaultServerPort, 2342,
				EP_HasMasterServer,
				EP_Done
			);
		}

		MasterClient *masterClient() const
		{
			return new ChocolateDoomMasterClient();
		}

		void registerIRCServer(QVector<IRCNetworkEntity> &networks) const
		{
			IRCNetworkEntity entity;
			entity.address = "irc.oftc.net";
			entity.description = "Chocolate Doom";
			entity.autojoinChannels << "#chocolate-doom";

			if(!networks.contains(entity))
				networks << entity;
		}

		Server* server(const QHostAddress &address, unsigned short port) const
		{
			return new ChocolateDoomServer(address, port);
		}
};
INSTALL_PLUGIN(ChocolateDoomEnginePlugin)
