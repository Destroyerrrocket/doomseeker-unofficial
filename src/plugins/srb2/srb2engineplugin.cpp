//------------------------------------------------------------------------------
// srb2engineplugin.cpp
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
#include "srb2engineplugin.h"

#include <QDateTime>

#include "srb2masterclient.h"
#include "srb2server.h"

INSTALL_PLUGIN(Srb2EnginePlugin)

Srb2EnginePlugin::Srb2EnginePlugin()
{
	const // clear warnings
	#include "srb2.xpm"

	init("Sonic Robo Blast 2", srb2_xpm,
		EP_Author, "The Doomseeker Team",
		EP_Version, 1,

		EP_DefaultMaster, "ms.srb2.org:28900",
		EP_DefaultServerPort, 5029,
		EP_MasterClient, new Srb2MasterClient(),
		EP_InGameFileDownloads,
#ifdef Q_OS_WIN32
		EP_ClientExeName, "srb2win",
#else
		EP_ClientExeName, "srb2",
#endif
		EP_GameFileSearchSuffixes, "srb2",
		EP_IRCChannel, "Sonic Robo Blast 2", "irc.esper.net", "#srb2fun",
		EP_Done
	);
}

ServerPtr Srb2EnginePlugin::mkServer(const QHostAddress &address, unsigned short port) const
{
	return ServerPtr(new Srb2Server(address, port));
}

QString Srb2::asciiOnly(const QByteArray &raw)
{
	QString result;
	for (int i = 0; i < raw.length() && raw[i] != '\0'; ++i)
	{
		char c = raw[i];
		if (c >= 0x20)
		{
			result += c;
		}
	}
	return result;
}
