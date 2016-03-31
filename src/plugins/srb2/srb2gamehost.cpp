//------------------------------------------------------------------------------
// srb2gamehost.cpp
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
// Copyright (C) 2016 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "srb2gamehost.h"

#include <serverapi/gamecreateparams.h>
#include <serverapi/serverstructs.h>
#include "srb2engineplugin.h"
#include "srb2gameinfo.h"

Srb2GameHost::Srb2GameHost()
: GameHost(Srb2EnginePlugin::staticInstance()),
  listenServer(false), casualServer(false)
{
	setArgForIwadLoading("-file");
	setArgForDemoPlayback("-playdemo");
	setArgForDemoRecord("-record");
	setArgForPort("-udpport");
	setArgForServerLaunch(""); // This will be determined basing on "casual" flag.
	set_addDMFlags(&Srb2GameHost::addDMFlags);
	set_addIwad(&Srb2GameHost::addIwad);
	set_addPwads(&Srb2GameHost::addPwads_prefixOnce);
}

void Srb2GameHost::addDMFlags()
{
	QList<DMFlagsSection> enabled = params().dmFlags();
	QList<DMFlagsSection> disabled = DMFlagsSection::removedBySection(
		Srb2GameInfo::dmFlags(), params().dmFlags());
	addDMFlags(enabled, true);
	addDMFlags(disabled, false);
}

void Srb2GameHost::addDMFlags(const QList<DMFlagsSection> &flags, bool enabled)
{
	foreach (const DMFlagsSection &section, flags)
	{
		for (int i = 0; i < section.count(); ++i)
		{
			DMFlag flag = section[i];
			switch (flag.value())
			{
			case Srb2GameInfo::ListenServer:
				listenServer = enabled;
				break;
			case Srb2GameInfo::CasualServer:
				casualServer = enabled;
				break;
			default:
			{
				QString command = Srb2GameInfo::commandFromFlag(
					static_cast<Srb2GameInfo::Flag>(flag.value()));
				if (!command.isEmpty())
				{
					args() << command << (enabled ? "1" : "0");
				}
			}
			}
		}
	}
}

void Srb2GameHost::addExtra()
{
	if (params().hostMode() == GameCreateParams::Host)
	{
		args() << (listenServer ? "-server" : "-dedicated");
	}

	unsigned int modeNum = params().gameMode().index();
	switch(params().gameMode().index())
	{
		default:
			modeNum = params().gameMode().index();
			break;
		case GameMode::SGM_Cooperative: modeNum = 0; break;
		case GameMode::SGM_Deathmatch: modeNum = 3; break;
		case GameMode::SGM_TeamDeathmatch: modeNum = 4; break;
		case GameMode::SGM_CTF: modeNum = 7; break;
	}
	args() << "-gametype" << QString::number(modeNum);
	args() << "+servername" << params().name();
	args() << "+maxplayers" << QString::number(params().maxPlayers());

	if (params().isBroadcastToMaster())
	{
		const QString STANDARD_ROOM = "33";
		const QString CASUAL_ROOM = "28";
		args() << "-room" << (casualServer ? CASUAL_ROOM : STANDARD_ROOM);
	}

	if (!params().map().isEmpty())
	{
		args() << "+map" << params().map();
	}

	if (!params().rconPassword().isEmpty())
	{
		args() << "-password" << params().rconPassword();
	}

	if (!params().motd().trimmed().isEmpty())
	{
		args() << "+motd" << params().motd().trimmed();
	}
}

void Srb2GameHost::addIwad()
{
	// No notion of IWAD.
}
