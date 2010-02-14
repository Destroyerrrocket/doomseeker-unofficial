//------------------------------------------------------------------------------
// vavoomserver.cpp
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

#include "vavoom/vavoomserver.h"
#include "main.h"
#include "serverapi/playerslist.h"

const // clear warnings
#include "vavoom/vavoom.xpm"

#define NET_PROTOCOL_VERSION	1
#define CCREQ_SERVER_INFO		2
#define CCREP_SERVER_INFO		13
#define NETPACKET_CTL			0x80

const QPixmap *VavoomServer::ICON = NULL;

const GameMode VavoomServer::GAME_MODES[NUM_VAVOOM_GAME_MODES] =
{
	GameMode(MODE_UNKNOWN, tr("Unknown"), false)
};

VavoomServer::VavoomServer(const QHostAddress &address, unsigned short port) : Server(address, port)
{
	currentGameMode = GAME_MODES[MODE_UNKNOWN];
}

QString VavoomServer::clientBinary(QString& error) const
{
	SettingsData* setting = Main::config->setting("VavoomBinaryPath");

	if (setting->string().isEmpty())
	{
		error = tr("No executable specified for Vavoom");
		return QString();
	}

	QFileInfo fi(setting->string());

	if (!fi.exists() || (fi.isDir() && !fi.isBundle()))
	{
		error = tr("%1\n is a directory or doesn't exist.").arg(setting->string());
		return QString();
	}

	return setting->string();
}

void VavoomServer::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const
{
	Server::connectParameters(args, pf, iwadFound, connectPassword);

	args[args.indexOf("-connect")] = "+connect"; // Change -connect to +connect for Vavoom

	// Remove original -iwad command
	int iwadArg = args.indexOf("-iwad");
	args.removeAt(iwadArg);
	args.removeAt(iwadArg);

	// What an odd thing to have to do "-iwaddir /path/to/iwads/ -doom2"
	QString iwadLocation = pf.findWad(iwadName().toLower());
	QString iwadDir = iwadLocation.left(iwadLocation.length() - iwadName().length());
	QString iwadParam = iwadLocation.mid(iwadDir.length());
	iwadParam.truncate(iwadParam.indexOf(QChar('.')));
	args << "-iwaddir";
	args << iwadDir;
	args << ("-" + iwadParam);

	args << Main::config->setting("VavoomCustomParameters")->string().split(" ", QString::SkipEmptyParts);
}

const QPixmap &VavoomServer::icon() const
{
	if(ICON == NULL)
		ICON = new QPixmap(vavoom_xpm);
	return *ICON;
}

Server::Response VavoomServer::readRequest(QByteArray &data)
{
	fflush(stderr);
	fflush(stdout);
	const char* in = data.data();

	// Check the response code
	int response = READINT8(&in[0]);
	int msgtype = READINT8(&in[1]);
	if(response != NETPACKET_CTL || msgtype != CCREP_SERVER_INFO)
	{
		return RESPONSE_BAD;
	}

	int pos = 2;

	// Server name
	serverName = QString(&in[pos+1]);//, in[pos]);
	pos += serverName.length() + 2;

	// Map
	mapName = QString(&in[pos+1]);//, in[pos]);
	pos += mapName.length() + 2;

	// Players
	players->clear();
	int numPlayers = READINT8(&in[pos++]);
	maxClients = maxPlayers = READINT8(&in[pos++]);
	for(int i = 0;i < numPlayers;i++)
	{
		Player player(tr("Unknown"), 0, 0, Player::TEAM_NONE);
		(*players) << player;
	}

	if(READINT8(&in[pos++]) != NET_PROTOCOL_VERSION)
		return RESPONSE_BAD;

	// Wads
	wads.clear();
	QString wadFile;
	bool iwadSet = false;
	while((wadFile = QString(&in[pos+1])) != "")
	{
		pos += wadFile.length() + 2;
		// Vavoom want you to know the exact location of the file on the server
		int lastSlash = wadFile.lastIndexOf(QChar('/')) > wadFile.lastIndexOf(QChar('\\')) ? wadFile.lastIndexOf(QChar('/')) : wadFile.lastIndexOf(QChar('\\'));
		if(lastSlash != -1)
			wadFile = wadFile.mid(lastSlash+1);

		if(!iwadSet)
		{
			iwad = wadFile;
			iwadSet = true;
		}
		else
			wads << wadFile;
	}
	pos += 2;

	return RESPONSE_GOOD;
}

bool VavoomServer::sendRequest(QByteArray &data)
{
	const char challenge[11] = { NETPACKET_CTL, CCREQ_SERVER_INFO, 6, 'V','A','V','O','O','M', 0, NET_PROTOCOL_VERSION };
	const QByteArray chall(challenge, 10);
	data.append(chall);
	return true;
}

QString VavoomServer::serverBinary(QString& error) const
{
	SettingsData* setting = Main::config->setting("VavoomServerBinaryPath");

	if (setting->string().isEmpty())
	{
		error = tr("No server executable specified for Vavoom");
		return QString();
	}

	QFileInfo fi(setting->string());

	if (!fi.exists() || (fi.isDir() && !fi.isBundle()))
	{
		error = tr("%1\nis a directory or doesn't exist.").arg(setting->string());
		return QString();
	}

	return setting->string();
}
