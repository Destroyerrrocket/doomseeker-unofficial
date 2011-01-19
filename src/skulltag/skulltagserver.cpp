//------------------------------------------------------------------------------
// skulltagserver.cpp
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

#include "skulltag/huffman/huffman.h"
#include "skulltag/skulltagbinaries.h"
#include "skulltag/skulltaggameinfo.h"
#include "skulltag/skulltaggamerunner.h"
#include "skulltag/skulltagserver.h"
#include "skulltag/skulltagmain.h"
#include "global.h"
#include "main.h"
#include "serverapi/playerslist.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QMessageBox>
#include <QRegExp>

const // clear warnings
#include "skulltag/skulltag.xpm"

#define SERVER_CHALLENGE	0xC7,0x00,0x00,0x00
#define SERVER_GOOD			5660023
#define SERVER_BANNED		5660025
#define SERVER_WAIT			5660024

#define RCON_PROTOCOL_VERSION	3

/**
 * Compares versions of Skulltag.
 */
class SkulltagVersion
{
	public:
		SkulltagVersion(QString version) : version(version)
		{
			versionExpression.exactMatch(version);
			QStringList parts = versionExpression.capturedTexts();
			major = parts[1].toUShort();
			minor = parts[2].toUShort();
			revision = parts[3][0].toAscii();
			build = parts[4].toUShort();
			tag = parts[5];
			svnRevision = parts[6].toUShort();
		}

		bool operator> (const SkulltagVersion &other) const
		{
			if(major > other.major && minor > other.minor && revision > other.revision && build > other.build)
				return true;
			if((tag.isEmpty() && !other.tag.isEmpty()) || (tag > other.tag))
				return true;
			if(svnRevision > other.svnRevision)
				return true;
			return false;
		}

	protected:
		static const QRegExp	versionExpression;
		QString					version;

		unsigned short int		major;
		unsigned short int		minor;
		unsigned char			revision;
		unsigned short int		build;
		QString					tag;
		unsigned short int		svnRevision;
};
const QRegExp SkulltagVersion::versionExpression("(\\d+).(\\d+)([a-zA-Z])(\\d*)(?:-([a-zA-Z]*)?)(?:-r(\\d+)?)");

////////////////////////////////////////////////////////////////////////////////

TeamInfo::TeamInfo(const QString &name, const QColor &color, unsigned int score) :
	teamName(name), teamColor(color), teamScore(score)
{
}

////////////////////////////////////////////////////////////////////////////////

const QPixmap *SkulltagServer::ICON = NULL;

SkulltagServer::SkulltagServer(const QHostAddress &address, unsigned short port)
: Server(address, port),
  buckshot(false), instagib(false), testingServer(false), teamDamage(0.0f),
  botSkill(0), duelLimit(0), fragLimit(0), pointLimit(0), winLimit(0),
  numTeams(2)
{
	teamInfo[0] = TeamInfo(tr("Blue"), QColor(0, 0, 255), 0);
	teamInfo[1] = TeamInfo(tr("Red"), QColor(255, 0, 0), 0);
	teamInfo[2] = TeamInfo(tr("Green"), QColor(0, 255, 0), 0);
	teamInfo[3] = TeamInfo(tr("Gold"), QColor(255, 255, 0), 0);

	connect(this, SIGNAL( updated(Server*, int) ), this, SLOT( updatedSlot(Server*, int) ));
}

Binaries* SkulltagServer::binaries() const
{
	return new SkulltagBinaries(this);
}

GameRunner* SkulltagServer::gameRunner() const
{
	return new SkulltagGameRunner(this);
}

const QPixmap &SkulltagServer::icon() const
{
	if(ICON == NULL)
		ICON = new QPixmap(skulltag_xpm);
	return *ICON;
}

unsigned int SkulltagServer::millisecondTime()
{
	const QTime time = QTime::currentTime();
	return time.hour()*360000 + time.minute()*60000 + time.second()*1000 + time.msec();
}

const GameCVar *SkulltagServer::modifier() const
{
	if(instagib)
		return &(*SkulltagGameInfo::gameModifiers())[1];
	else if(buckshot)
		return &(*SkulltagGameInfo::gameModifiers())[0];
	return NULL;
}

const PluginInfo* SkulltagServer::plugin() const
{
	return SkulltagMain::get();
}

Server::Response SkulltagServer::readRequest(QByteArray &data)
{
	const int OUT_SIZE = 6000;
	// Decompress the response.
	const char* in = data.data();
	char packetOut[OUT_SIZE];
	int out = OUT_SIZE;

	HUFFMAN_Decode(reinterpret_cast<const unsigned char*> (in), reinterpret_cast<unsigned char*> (packetOut), data.size(), &out);

	lastReadRequest.clear();
	lastReadRequest = QByteArray(packetOut, qMin(out, OUT_SIZE));

	if (out == OUT_SIZE)
	{
		fprintf(stderr, "Warning (probably an error): data size error when reading server %s:%u. Data size is IN: %u, OUT: %u\n", address().toString().toAscii().constData(), port(), data.size(), out);
	}

	if (out < 4 || out > OUT_SIZE)
	{
		fprintf(stderr, "Data size error when reading server %s:%u. Data size is IN: %u, OUT: %u\n", address().toString().toAscii().constData(), port(), data.size(), out);
		return RESPONSE_BAD;
	}

	// Check the response code
	int response = READINT32(&packetOut[0]);

	// Determine ping. Time is sent no matter what the response is, still we
	// should check if there's enough data to read from.
	if (out >= 8)
	{
		currentPing = millisecondTime() - READINT32(&packetOut[4]);
		bPingIsSet = true;
	}
	else
	{
		return RESPONSE_BAD;
	}

	// Act according to the response
	switch(response)
	{
		case SERVER_BANNED:
			return RESPONSE_BANNED;

		case SERVER_WAIT:
			return RESPONSE_WAIT;

		case SERVER_GOOD:
			// Do nothing, continue
			break;

		default:
			return RESPONSE_BAD;
	}

	// If response was equal to SERVER_GOOD, proceed to read data.
	serverVersion = QString(&packetOut[8]);
	int pos = 8 + serverVersion.length() + 1;

	// now read the data.
	SkulltagGameInfo::SkulltagGameMode mode = SkulltagGameInfo::GAMEMODE_COOPERATIVE;

	// Flags - set of flags returned by the server. This is compared
	// with known set of flags and the data is read from the packet
	// accordingly. Every flag is removed from this variable after such check.
	// See "if SQF_NAME" for an example.
	unsigned flags = READINT32(&packetOut[pos]);
	pos += 4;
	if((flags & SQF_NAME) == SQF_NAME) // Check if SQF_NAME is inside flags var.
	{
		serverName = QString(&packetOut[pos]); // Read the data.
		pos += serverName.length() + 1; // Move on to next data.
		flags ^= SQF_NAME; // Remove SQF_NAME flag from the variable.

		if (pos >= out && flags != 0)
		{
			return RESPONSE_BAD;
		}
	}

	if((flags & SQF_URL) == SQF_URL)
	{
		webSite = QString(&packetOut[pos]);
		pos += webSite.length() + 1;
		flags ^= SQF_URL;

		if (pos >= out && flags != 0)
		{
			return RESPONSE_BAD;
		}
	}
	if((flags & SQF_EMAIL) == SQF_EMAIL)
	{
		email = QString(&packetOut[pos]);
		pos += email.length() + 1;
		flags ^= SQF_EMAIL;

		if (pos >= out && flags != 0)
		{
			return RESPONSE_BAD;
		}
	}
	if((flags & SQF_MAPNAME) == SQF_MAPNAME)
	{
		mapName = QString(&packetOut[pos]);
		pos += mapName.length() + 1;
		flags ^= SQF_MAPNAME;

		if (pos >= out && flags != 0)
		{
			return RESPONSE_BAD;
		}
	}

	if((flags & SQF_MAXCLIENTS) == SQF_MAXCLIENTS)
	{
		maxClients = READINT8(&packetOut[pos++]);
		flags ^= SQF_MAXCLIENTS;
	}
	else
	{
		maxClients = 0;
	}

	if((flags & SQF_MAXPLAYERS) == SQF_MAXPLAYERS)
	{
		maxPlayers = READINT8(&packetOut[pos++]);
		flags ^= SQF_MAXPLAYERS;
	}
	else
	{
		maxPlayers = 0;
	}

	if((flags & SQF_PWADS) == SQF_PWADS)
	{
		int numPwads = READINT8(&packetOut[pos++]);
		wads.clear(); // clear any previous list we may have had.
		flags ^= SQF_PWADS;
		for(int i = 0;i < numPwads;i++)
		{
			QString wad(&packetOut[pos]);
			pos += wad.length() + 1;
			wads << wad;

			// Check everytime if pos has reached the end of array.
			// Emit response RESPONSE_BAD if it did and there are flags
			// remaining in the variable. Following code is used repeatedly
			// in this method.
			if (pos >= out && flags != 0)
			{
				return RESPONSE_BAD;
			}
		}
	}

	if((flags & SQF_GAMETYPE) == SQF_GAMETYPE)
	{
		unsigned char byMode = READINT8(&packetOut[pos++]);

		if (byMode > NUM_SKULLTAG_GAME_MODES)
		{
			byMode = NUM_SKULLTAG_GAME_MODES; // this will set game mode to unknown
		}

		mode = static_cast<SkulltagGameInfo::SkulltagGameMode> (byMode);
		currentGameMode = (*SkulltagGameInfo::gameModes())[mode];
		instagib = READINT8(&packetOut[pos++]) != 0;
		buckshot = READINT8(&packetOut[pos++]) != 0;

		flags ^= SQF_GAMETYPE;
	}

	if((flags & SQF_GAMENAME) == SQF_GAMENAME)
	{
		//Useless String
		pos += strlen(&packetOut[pos]) + 1;
		flags ^= SQF_GAMETYPE;

		if (pos >= out && flags != 0)
		{
			return RESPONSE_BAD;
		}
	}

	if((flags & SQF_IWAD) == SQF_IWAD)
	{
		iwad = QString(&packetOut[pos]);
		pos += iwad.length() + 1;
		flags ^= SQF_IWAD;

		if (pos >= out && flags != 0)
		{
			return RESPONSE_BAD;
		}
	}

	if((flags & SQF_FORCEPASSWORD) == SQF_FORCEPASSWORD)
	{
		locked = (READINT8(&packetOut[pos++]) != 0);
		flags ^= SQF_FORCEPASSWORD;
	}
	else
	{
		locked = false;
	}

	if((flags & SQF_FORCEJOINPASSWORD) == SQF_FORCEJOINPASSWORD)
	{
		pos++;
		flags ^= SQF_FORCEJOINPASSWORD;
	}

	if((flags & SQF_GAMESKILL) == SQF_GAMESKILL)
	{
		skill = READINT8(&packetOut[pos++]);
		flags ^= SQF_GAMESKILL;
	}

	if((flags & SQF_BOTSKILL) == SQF_BOTSKILL)
	{
		botSkill = READINT8(&packetOut[pos++]);
		flags ^= SQF_BOTSKILL;
	}

	if((flags & SQF_DMFLAGS) == SQF_DMFLAGS)
	{
		flags ^= SQF_DMFLAGS;

		clearDMFlags();

		const DMFlags& skulltagFlags = *SkulltagGameInfo::dmFlags();

		// Read each dmflags section separately.
		for (int i = 0; i < NUM_DMFLAG_SECTIONS; ++i)
		{
			unsigned int dmflags = READINT32(&packetOut[pos]);
			pos += 4;

			const DMFlagsSection& skulltagFlagsSection = *skulltagFlags[i];
			DMFlagsSection* dmFlagsSection = new DMFlagsSection();
			dmFlagsSection->name = skulltagFlagsSection.name;

			// Iterate through every known flag to check whether it should be
			// inserted into the structure of this server.
			for (int j = 0; j < skulltagFlagsSection.flags.count(); ++j)
			{
				if ( (dmflags & (1 << skulltagFlagsSection.flags[j].value)) != 0)
				{
					dmFlagsSection->flags << skulltagFlagsSection.flags[j];
				}
			}

			dmFlags << dmFlagsSection;
		}
	}

	if((flags & SQF_LIMITS) == SQF_LIMITS)
	{
		flags ^= SQF_LIMITS;
		fragLimit = READINT16(&packetOut[pos]);

		// Read timelimit and timeleft,
		// note that if timelimit == 0 then no info
		// about timeleft is sent
		serverTimeLimit = READINT16(&packetOut[pos+2]);
		pos += 4;
		if (serverTimeLimit != 0)
		{
			serverTimeLeft = READINT16(&packetOut[pos]);
			pos += 2;
		}

		duelLimit = READINT16(&packetOut[pos]);
		pointLimit = READINT16(&packetOut[pos+2]);
		winLimit = READINT16(&packetOut[pos+4]);
		switch(mode)
		{
			default:
				serverScoreLimit = fragLimit;
				break;
			case SkulltagGameInfo::GAMEMODE_LASTMANSTANDING:
			case SkulltagGameInfo::GAMEMODE_TEAMLMS:
				serverScoreLimit = winLimit;
				break;
			case SkulltagGameInfo::GAMEMODE_POSSESSION:
			case SkulltagGameInfo::GAMEMODE_TEAMPOSSESSION:
			case SkulltagGameInfo::GAMEMODE_TEAMGAME:
			case SkulltagGameInfo::GAMEMODE_CTF:
			case SkulltagGameInfo::GAMEMODE_ONEFLAGCTF:
			case SkulltagGameInfo::GAMEMODE_SKULLTAG:
			case SkulltagGameInfo::GAMEMODE_DOMINATION:
				serverScoreLimit = pointLimit;
				break;
		}
		pos += 6;
	}
	else
	{
		// Nullify vars if there is no info
		fragLimit = 0;
		serverTimeLimit = 0;
		duelLimit = 0;
		pointLimit = 0;
		winLimit = 0;
		serverScoreLimit = 0;
	}

	if((flags & SQF_TEAMDAMAGE) == SQF_TEAMDAMAGE)
	{
		teamDamage = QByteArray(&packetOut[pos], 4).toFloat();
		pos += 4;
		flags ^= SQF_TEAMDAMAGE;
	}
	if((flags & SQF_TEAMSCORES) == SQF_TEAMSCORES)
	{
		// DEPRECATED flag
		for(int i = 0;i < 2;i++)
		{
			scores[i] = READINT16(&packetOut[pos]);
			pos += 2;
		}
		flags ^= SQF_TEAMSCORES;
	}
	if((flags & SQF_NUMPLAYERS) == SQF_NUMPLAYERS)
	{
		int numPlayers = READINT8(&packetOut[pos++]);
		flags ^= SQF_NUMPLAYERS;

		// If number of players is bigger than number of maximum clients
		// we assume something went horribly wrong and emit an error signal.
		if (numPlayers > maxClients)
		{
			return RESPONSE_BAD;
		}

		if((flags & SQF_PLAYERDATA) == SQF_PLAYERDATA)
		{
			flags ^= SQF_PLAYERDATA;
			players->clear(); // Erase previous players (if any)
			for(int i = 0;i < numPlayers;i++)
			{
				// team isn't sent in non team modes.
				bool teammode = currentGameMode.isTeamGame();

				QString name(&packetOut[pos]);
				pos += name.length() + 1;

				if (pos >= out)
				{
					return RESPONSE_BAD;
				}

				int score = READINT16(&packetOut[pos]);
				int ping = READINT16(&packetOut[pos+2]);
				bool spectating = READINT8(&packetOut[pos+4]) != 0;
				bool bot = READINT8(&packetOut[pos+5]) != 0;
				int team = teammode ? READINT8(&packetOut[pos+6]) : Player::TEAM_NONE;
				// Unused:
				// int time = READINT8(&packetOut[pos+(teammode ? 7 : 6)]);
				pos += teammode ? 8 : 7;

				Player player(name, score, ping, static_cast<Player::PlayerTeam> (team), spectating, bot);
				*players << player;
			}
		}
	}

	if((flags & SQF_TEAMINFO_NUMBER) == SQF_TEAMINFO_NUMBER)
	{
		numTeams = READINT8(&packetOut[pos++]);
		flags ^= SQF_TEAMINFO_NUMBER;
	}

	if((flags & SQF_TEAMINFO_NAME) == SQF_TEAMINFO_NAME)
	{
		flags ^= SQF_TEAMINFO_NAME;
		for(unsigned i = 0; i < numTeams && i < ST_MAX_TEAMS; ++i)
		{
			teamInfo[i].setName(tr(&packetOut[pos]));
			pos += teamInfo[i].name().length() + 1;
			if (pos >= out && (i != numTeams - 1 || flags != 0) )
			{
				return RESPONSE_BAD;
			}
		}
	}
	if((flags & SQF_TEAMINFO_COLOR) == SQF_TEAMINFO_COLOR)
	{
		flags ^= SQF_TEAMINFO_COLOR;
		// NOTE: This may not be correct
		unsigned forLimit = qMin(numTeams, ST_MAX_TEAMS);

		for(unsigned i = 0; i < forLimit; i++)
		{
			teamInfo[i].setColor(QColor(READINT32(&packetOut[pos])));
			pos += 4;

			if (pos >= out && (i != forLimit - 1 || flags != 0))
			{
				return RESPONSE_BAD;
			}
		}
	}
	if((flags & SQF_TEAMINFO_SCORE) == SQF_TEAMINFO_SCORE)
	{
		flags ^= SQF_TEAMINFO_SCORE;
		unsigned forLimit = qMin(numTeams, ST_MAX_TEAMS);

		for(unsigned i = 0; i < forLimit; i++)
		{
			teamInfo[i].setScore(READINT16(&packetOut[pos]));
			if(i < MAX_TEAMS) // Transfer to super class score array if possible.
				scores[i] = teamInfo[i].score();
			pos += 2;

			if (pos >= out && (i != forLimit - 1 || flags != 0))
			{
				return RESPONSE_BAD;
			}
		}
	}


	// Due to a bug in 0.97d3 we need to add additional checks here.
	// 0.97d3 servers also respond with SQF_TESTING_SERVER flag set
	// if it was previously sent to them
	if (pos < out && SkulltagVersion(version()) > SkulltagVersion("0.97d3") && (flags & SQF_TESTING_SERVER) == SQF_TESTING_SERVER)
	{
		flags ^= SQF_TESTING_SERVER;
		testingServer = static_cast<bool>(READINT8(&packetOut[pos]));
		++pos;

		// '\0' is read if testingServer == false
		testingArchive = &packetOut[pos];
		pos += testingArchive.length() + 1;
	}
	else
	{
		testingServer = false;
		testingArchive = QString();
	}

	return RESPONSE_GOOD;
}

bool SkulltagServer::sendRequest(QByteArray &data)
{
	// Send launcher challenge.
	int query = SQF_STANDARDQUERY;
//	const unsigned char challenge[12] = {SERVER_CHALLENGE, WRITEINT32_DIRECT(query), 0x00, 0x00, 0x00, 0x00};
	const unsigned char challenge[12] = {SERVER_CHALLENGE, WRITEINT32_DIRECT(query), WRITEINT32_DIRECT(millisecondTime()) };
	char challengeOut[16];
	int out = 16;
	HUFFMAN_Encode(challenge, reinterpret_cast<unsigned char*> (challengeOut), 12, &out);
	const QByteArray chall(challengeOut, out);
	data.append(chall);
	return true;
}

QRgb SkulltagServer::teamColor(unsigned team) const
{
	if(team >= ST_MAX_TEAMS || team < 0)
		return Server::teamColor(team);

	return teamInfo[team].color().rgb();
}

QString	SkulltagServer::teamName(unsigned team) const
{
	if (team == 255)
		return "NO TEAM";

	return team >= 0 && team < ST_MAX_TEAMS ? teamInfo[team].name() : "";
}

void SkulltagServer::updatedSlot(Server* server, int response)
{
	if (response == RESPONSE_BAD)
	{
		// If response is bad we will print the read request to stderr,
		// for debug purposes of course.
		SkulltagServer* s = (SkulltagServer*)server;
		QByteArray& req = s->lastReadRequest;

		fprintf(stderr, "Bad response from server: %s:%u\n", address().toString().toAscii().constData(), port());
		fprintf(stderr, "Response size: %u\n", req.size());
		fprintf(stderr, "Data (all non-printable characters are replaced with '?'):\n");


		for (int x = 0; x < req.size(); ++x)
		{
			if (req[x] < (char)0x20 || req[x] > (char)0x7e)
			{
				fprintf(stderr, "?");
			}
			else
			{
				fprintf(stderr, "%c", (char)req[x]);
			}
			fprintf(stderr, "[%02X] ", (unsigned char)req[x]);

			if ((x + 1) % 20 == 0)
			{
				fprintf(stderr, "\n");
			}
		}

		fprintf(stderr, "\n\n");

		fprintf(stderr, "Data as text (all non-printable characters are replaced with '?'):\n");
		for (int x = 0; x < req.size(); ++x)
		{
			if (req[x] < (char)0x20 || req[x] > (char)0x7e)
			{
				fprintf(stderr, "?");
			}
			else
			{
				fprintf(stderr, "%c", (char)req[x]);
			}

			if ((x + 1) % 70 == 0)
			{
				fprintf(stderr, "\n");
			}
		}

		fprintf(stderr, "\n-- End of response --\n\n");
	}
}

////////////////////////////////////////////////////////////////////////////////

SkulltagRConProtocol::SkulltagRConProtocol(Server *server) : RConProtocol(server)
{
	// Note: the original rcon utility did TIMEOUT/4.
	// Try to get at least 4 packets in before timing out,
	pingTimer.setInterval(2500);
	connect(&pingTimer, SIGNAL( timeout() ), this, SLOT( sendPong() ));
}

RConProtocol *SkulltagRConProtocol::connectToServer(Server *server)
{
	SkulltagRConProtocol *protocol = new SkulltagRConProtocol(server);

	const unsigned char beginConnection[2] = { CLRC_BEGINCONNECTION, RCON_PROTOCOL_VERSION };
	char encodedConnection[4];
	int encodedSize = 4;
	HUFFMAN_Encode(beginConnection, reinterpret_cast<unsigned char*> (encodedConnection), 2, &encodedSize);

	// Try to connect, up to 3 times
	protocol->connected = false;
	for(unsigned int attempts = 0;attempts < 3;attempts++)
	{
		protocol->socket.writeDatagram(encodedConnection, encodedSize, server->address(), server->port());
		if(protocol->socket.waitForReadyRead(3000))
		{
			int size = protocol->socket.pendingDatagramSize();
			char* data = new char[size];
			protocol->socket.readDatagram(data, size);
			char packet[64];
			int decodedSize = 64;
			HUFFMAN_Decode(reinterpret_cast<const unsigned char*> (data), reinterpret_cast<unsigned char*> (packet), size, &decodedSize);
			delete[] data;
			switch(packet[0])
			{
				case SVRC_BANNED:
					QMessageBox::critical(NULL, tr("Banned"), tr("You have been banned from this server."));
					break;
				default:
				case SVRC_OLDPROTOCOL:
					QMessageBox::critical(NULL, tr("Incompatible Protocol"), tr("The protocol appears to be outdated."));
					break;
				case SVRC_SALT:
					protocol->connected = true;
					protocol->salt = QString(&packet[1]);
					protocol->pingTimer.start();
					return protocol;
			}
			break;
		}
	}
	delete protocol;
	return NULL;
}

void SkulltagRConProtocol::disconnectFromServer()
{
	const unsigned char disconnectPacket[1] = { CLRC_DISCONNECT };
	char encodedDisconnect[4];
	int encodedSize = 4;
	HUFFMAN_Encode(disconnectPacket, reinterpret_cast<unsigned char*> (encodedDisconnect), 1, &encodedSize);
	socket.writeDatagram(encodedDisconnect, encodedSize, server->address(), server->port());
	connected = false;
	pingTimer.stop();
	emit disconnected();
}

void SkulltagRConProtocol::sendCommand(const QString &cmd)
{
	unsigned char packet[4096];
	packet[0] = CLRC_COMMAND;
	packet[cmd.length()+1] = 0;
	memcpy(packet+1, cmd.toAscii().constData(), cmd.length());
	char encodedPacket[4097];
	int encodedSize = 4097;
	HUFFMAN_Encode(packet, reinterpret_cast<unsigned char*> (encodedPacket), cmd.length()+2, &encodedSize);
	socket.writeDatagram(encodedPacket, encodedSize, server->address(), server->port());
}

void SkulltagRConProtocol::sendPassword(const QString &password)
{
	// Calculate the MD5 of the salt + password
	QString hashPassword = salt + password;
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(hashPassword.toAscii());
	QByteArray md5 = hash.result().toHex();

	// Create the packet
	unsigned char passwordPacket[34];
	passwordPacket[0] = CLRC_PASSWORD;
	memcpy(passwordPacket+1, md5.data(), md5.size());
	passwordPacket[33] = 0;
	char encodedPassword[50];
	int encodedLength = 50;
	HUFFMAN_Encode(passwordPacket, reinterpret_cast<unsigned char*> (encodedPassword), 34, &encodedLength);

	for(unsigned int i = 0;i < 3;i++)
	{
		socket.writeDatagram(encodedPassword, encodedLength, server->address(), server->port());

		if(socket.waitForReadyRead(3000))
		{
			packetReady();
			connect(&socket, SIGNAL( readyRead() ), this, SLOT( packetReady() ));
			break;
		}
	}
}

void SkulltagRConProtocol::sendPong()
{
	// create a "PONG" packet
	const unsigned char pong[1] = { CLRC_PONG };
	char encodedPong[4];
	int encodedSize = 4;
	HUFFMAN_Encode(pong, reinterpret_cast<unsigned char*> (encodedPong), 1, &encodedSize);
	socket.writeDatagram(encodedPong, encodedSize, server->address(), server->port());
}

void SkulltagRConProtocol::packetReady()
{
	if(!connected)
		return;

	while(socket.hasPendingDatagrams())
	{
		int size = socket.pendingDatagramSize();
		char* data = new char[size];
		socket.readDatagram(data, size);
		char packet[4096];
		int decodedSize = 4096;
		HUFFMAN_Decode(reinterpret_cast<const unsigned char*> (data), reinterpret_cast<unsigned char*> (packet), size, &decodedSize);
		delete[] data;

		processPacket(packet, size);
	}
}

void SkulltagRConProtocol::processPacket(const char *data, int length, bool initial, int maxUpdates, int *pos)
{
	if(length <= 0)
		return;

	int position = 0;
	while(position < length && maxUpdates-- != 0)
	{
		// Determine how we get to the update.
		int update = 0;
		if(initial)
			update = SVRC_UPDATE;
		else
			update = data[position++];

		switch(update)
		{
			default:
				qDebug() << "Unknown update (" << static_cast<int> (data[position-1]) << ")";
				if(pos != NULL)
					*pos = position;
				return;
			case SVRC_INVALIDPASSWORD:
				emit invalidPassword();
				break;
			case SVRC_LOGGEDIN:
			{
				connect(&socket, SIGNAL( readyRead() ), this, SLOT( packetReady() ));
				serverProtocolVersion = data[1];
				hostName = QString(&data[2]);
				emit serverNameChanged(hostName);
				int numUpdates = data[hostName.length() + 3];
				int position = 0;
				processPacket(data + hostName.length() + 4, length - hostName.length() - 4, true, numUpdates, &position);
				position += hostName.length() + 4;
				int numStrings = data[position++];
				while(numStrings-- > 0)
				{
					QString message(&data[position]);
					position += message.length() + 1;
					emit messageReceived(message.trimmed());
				}
				break;
			}
			case SVRC_MESSAGE:
			{
				QString message = QDateTime::currentDateTime().toString("[hh:mm:ss ap] ") + QString(&data[position]);
				position += message.length() + 1;
				emit messageReceived(message);
				break;
			}
			case SVRC_UPDATE:
				switch(data[position++])
				{
					default:
						qDebug() << "Uknown streamlined update (" << static_cast<int> (data[position-1]) << ")";
						if(pos != NULL)
							*pos = position;
						return;
					case SVRCU_MAP:
					{
						QString map = QString(&data[position]);
						position += map.length() + 1;
						break;
					}
					case SVRCU_ADMINCOUNT:
					{
						// Unused:
						// int admins = data[position++];
						// !!! MAKE SURE to remove the line below if line above
						// is uncommented!
						position++;
						break;
					}
					case SVRCU_PLAYERDATA:
					{
						int players = data[position++];
						this->players.clear();
						while(players-- > 0)
						{
							QString player(&data[position]);
							position += player.length() + 1;
							this->players.append(Player(player, 0, 0));
						}
						emit playerListUpdated();
						break;
					}
				}
				break;
		}
	}
	if(pos != NULL)
		*pos = position;
}

RConProtocol *SkulltagServer::rcon()
{
	return SkulltagRConProtocol::connectToServer(this);
}
