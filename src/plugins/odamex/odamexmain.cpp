//------------------------------------------------------------------------------
// odamexmain.cpp
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

#include <QDateTime>

#include "irc/entities/ircnetworkentity.h"
#include "plugins/engineplugin.h"

#include "odamexgameinfo.h"
#include "odamexmasterclient.h"
#include "odamexmain.h"
#include "odamexserver.h"

EnginePlugin *OdamexMain::info;

class OdamexEnginePlugin : public EnginePlugin
{
	public:
		OdamexEnginePlugin()
		{
			static const unsigned int NUM_MASTERS = 2;
			static const char* masters[NUM_MASTERS] = {
				"master1.odamex.net:15000", "master2.odamex.net:15000"
			};
			qsrand(QDateTime::currentMSecsSinceEpoch());

			OdamexMain::info = this;

			const // clear warnings
			#include "odamex.xpm"

			init("Odamex", odamex_xpm,
				EP_Author, "The Doomseeker Team",
				EP_Version, 7,

				EP_AllowsURL,
				EP_AllowsEmail,
				EP_AllowsConnectPassword,
				EP_AllowsJoinPassword,
				EP_AllowsMOTD,
				EP_DefaultMaster, masters[qrand()%NUM_MASTERS],
				EP_DefaultServerPort, 10666,
				EP_GameModes, OdamexGameInfo::gameModes(),
				EP_HasMasterServer,
				EP_IRCChannel, "Odamex", "irc.oftc.net", "#odamex",
				EP_SupportsRandomMapRotation,
				EP_Done
			);
		}

		MasterClient *masterClient() const
		{
			return new OdamexMasterClient();
		}

		Server* server(const QHostAddress &address, unsigned short port) const
		{
			return new OdamexServer(address, port);
		}
};
INSTALL_PLUGIN(OdamexEnginePlugin)
