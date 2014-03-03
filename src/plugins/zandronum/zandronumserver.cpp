//------------------------------------------------------------------------------
// zandronumserver.cpp
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "zandronumserver.h"

#include "huffman/huffman.h"
#include "zandronumbinaries.h"
#include "zandronumgamehost.h"
#include "zandronumgameinfo.h"
#include "zandronumgamerunner.h"
#include "zandronumengineplugin.h"
#include "global.h"
#include "log.h"
#include "datastreamoperatorwrapper.h"
#include "serverapi/playerslist.h"

#include <QBuffer>
#include <QCryptographicHash>
#include <QDataStream>
#include <QDateTime>
#include <QMessageBox>
#include <QRegExp>

#define SERVER_CHALLENGE	0xC7,0x00,0x00,0x00
#define SERVER_GOOD			5660023
#define SERVER_BANNED		5660025
#define SERVER_WAIT			5660024

#define RCON_PROTOCOL_VERSION	3

#define RETURN_BAD_IF_NOT_ENOUGH_DATA(min_amout_of_data_required) \
{ \
	/* qDebug() << "pos/size" << in.device()->pos() << "/" << in.device()->size(); */\
	if (in.remaining() < (min_amout_of_data_required)) \
	{ \
		return RESPONSE_BAD; \
	} \
}

/**
 * Compares versions of Zandronum.
 */
ZandronumVersion::ZandronumVersion(QString version) : version(version)
{
	versionExpression.exactMatch(version);
	QStringList parts = versionExpression.capturedTexts();
	major = parts[1].toUShort();
	minor = parts[2].toUShort();
	revision = parts[3][0].toAscii();
	build = parts[4].toUShort();
	tag = parts[5];
	hgRevisionDate = parts[6].toUInt();
	hgRevisionTime = parts[7].toUShort();
}

bool ZandronumVersion::operator> (const ZandronumVersion &other) const
{
	if(major > other.major && minor > other.minor && revision > other.revision && build > other.build)
		return true;
	if((tag.isEmpty() && !other.tag.isEmpty()) || (tag > other.tag))
		return true;
	if(hgRevisionDate > other.hgRevisionDate)
		return true;
	if(hgRevisionTime > other.hgRevisionTime)
		return true;
	return false;
}

const QRegExp ZandronumVersion::versionExpression("(\\d+).(\\d+)([a-zA-Z]?)(\\d*)(?:-([a-zA-Z]*)?)?(?:-r(\\d+)(?:-(\\d+))?)?");

////////////////////////////////////////////////////////////////////////////////

TeamInfo::TeamInfo(const QString &name, const QColor &color, unsigned int score) :
	teamName(name), teamColor(color), teamScore(score)
{
}

////////////////////////////////////////////////////////////////////////////////

ZandronumServer::ZandronumServer(const QHostAddress &address, unsigned short port)
: Server(address, port),
  buckshot(false), instagib(false), testingServer(false), teamDamage(0.0f),
  botSkill(0), duelLimit(0), fragLimit(0), pointLimit(0), winLimit(0),
  numTeams(2)
{
	teamInfo[0] = TeamInfo(tr("Blue"), QColor(0, 0, 255), 0);
	teamInfo[1] = TeamInfo(tr("Red"), QColor(255, 0, 0), 0);
	teamInfo[2] = TeamInfo(tr("Green"), QColor(0, 255, 0), 0);
	teamInfo[3] = TeamInfo(tr("Gold"), QColor(255, 255, 0), 0);

	set_createSendRequest(&ZandronumServer::createSendRequest);
	set_readRequest(&ZandronumServer::readRequest);

	connect(this, SIGNAL( updated(ServerPtr, int) ), this, SLOT( updatedSlot(ServerPtr, int) ));
}

ExeFile* ZandronumServer::clientExe()
{
	return new ZandronumClientExeFile(self().toStrongRef().staticCast<ZandronumServer>());
}

GameClientRunner* ZandronumServer::gameRunner()
{
	return new ZandronumGameClientRunner(self());
}

unsigned int ZandronumServer::millisecondTime()
{
	const QTime time = QTime::currentTime();
	return time.hour()*360000 + time.minute()*60000 + time.second()*1000 + time.msec();
}

QList<GameCVar> ZandronumServer::modifiers() const
{
	QList<GameCVar> result;
	if(instagib)
		result << (*ZandronumGameInfo::gameModifiers())[1];
	else if(buckshot)
		result << (*ZandronumGameInfo::gameModifiers())[0];
	return result;
}

EnginePlugin* ZandronumServer::plugin() const
{
	return ZandronumEnginePlugin::staticInstance();
}

Server::Response ZandronumServer::readRequest(const QByteArray &data)
{
	const int BUFFER_SIZE = 6000;
	QByteArray rawReadBuffer;

	// Decompress the response.
	const char* huffmanPacket = data.data();

	int decodedSize = BUFFER_SIZE + data.size();
	char* packetDecodedBuffer = new char[decodedSize];

	HUFFMAN_Decode(reinterpret_cast<const unsigned char*> (huffmanPacket),
		reinterpret_cast<unsigned char*> (packetDecodedBuffer), data.size(),
		&decodedSize);

	if (decodedSize <= 0)
	{
		delete [] packetDecodedBuffer;
		return RESPONSE_BAD;
	}

	// Prepare reading interface.
	QByteArray packetDecoded(packetDecodedBuffer, decodedSize);
	lastReadRequest = packetDecoded;

	delete [] packetDecodedBuffer;

	QBuffer packetDecodedIo(&packetDecoded);

	packetDecodedIo.open(QIODevice::ReadOnly);
	packetDecodedIo.seek(0);

	QDataStream inStream(&packetDecodedIo);
	inStream.setByteOrder(QDataStream::LittleEndian);

	DataStreamOperatorWrapper in(&inStream);

	// Read and parse.

	// Do the initial sanity check. All packets must be at least 8 bytes big.
	if (decodedSize < 8)
	{
		fprintf(stderr, "Data size error when reading server %s:%u."
			" Data size encoded: %u, decoded %u\n",
			address().toString().toAscii().constData(), port(),
			data.size(), decodedSize);
		return RESPONSE_BAD;
	}

	// Check the response code
	qint32 response = in.readQInt32();

	// Determine ping. Time is sent no matter what the response is, still we
	// should check if there's enough data to read from.
	qint32 responseTimestamp = in.readQInt32();
	setPing(millisecondTime() - responseTimestamp);
	setPingIsSet(true);

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
	RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
	setGameVersion(in.readRawUntilByte('\0'));

	ZandronumGameInfo::ZandronumGameMode mode = ZandronumGameInfo::GAMEMODE_COOPERATIVE;

	// Flags - set of flags returned by the server. This is compared
	// with known set of flags and the data is read from the packet
	// accordingly. Every flag is removed from this variable after such check.
	// See "if SQF_NAME" for an example.
	RETURN_BAD_IF_NOT_ENOUGH_DATA(4);
	quint32 flags = in.readQUInt32();
	if((flags & SQF_NAME) == SQF_NAME) // Check if SQF_NAME is inside flags var.
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		setName(in.readRawUntilByte('\0'));
		flags ^= SQF_NAME; // Remove SQF_NAME flag from the variable.
	}

	if((flags & SQF_URL) == SQF_URL)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		setWebSite(in.readRawUntilByte('\0'));
		flags ^= SQF_URL;
	}
	if((flags & SQF_EMAIL) == SQF_EMAIL)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		setEmail(in.readRawUntilByte('\0'));
		flags ^= SQF_EMAIL;
	}
	if((flags & SQF_MAPNAME) == SQF_MAPNAME)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		setMap(in.readRawUntilByte('\0'));
		flags ^= SQF_MAPNAME;
	}

	if((flags & SQF_MAXCLIENTS) == SQF_MAXCLIENTS)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		setMaxClients(in.readQUInt8());
		flags ^= SQF_MAXCLIENTS;
	}
	else
	{
		setMaxClients(0);
	}

	if((flags & SQF_MAXPLAYERS) == SQF_MAXPLAYERS)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		setMaxPlayers(in.readQUInt8());
		flags ^= SQF_MAXPLAYERS;
	}
	else
	{
		setMaxPlayers(0);
	}

	if((flags & SQF_PWADS) == SQF_PWADS)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		qint8 numPwads = in.readQInt8();
		clearWads();
		flags ^= SQF_PWADS;
		for(int i = 0; i < numPwads; i++)
		{
			RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
			QString wad = in.readRawUntilByte('\0');
			addWad(wad);
		}
	}

	if((flags & SQF_GAMETYPE) == SQF_GAMETYPE)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		qint8 modeCode = in.readQInt8();

		if (modeCode > NUM_ZANDRONUM_GAME_MODES)
		{
			modeCode = NUM_ZANDRONUM_GAME_MODES; // this will set game mode to unknown
		}

		mode = static_cast<ZandronumGameInfo::ZandronumGameMode> (modeCode);
		setGameMode((*ZandronumGameInfo::gameModes())[mode]);

		RETURN_BAD_IF_NOT_ENOUGH_DATA(2);
		instagib = in.readQInt8() != 0;
		buckshot = in.readQInt8() != 0;

		flags ^= SQF_GAMETYPE;
	}

	if((flags & SQF_GAMENAME) == SQF_GAMENAME)
	{
		//Useless String
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		in.readRawUntilByte('\0');
	}

	if((flags & SQF_IWAD) == SQF_IWAD)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		setIwad(in.readRawUntilByte('\0'));
		flags ^= SQF_IWAD;
	}

	if((flags & SQF_FORCEPASSWORD) == SQF_FORCEPASSWORD)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		setLocked(in.readQInt8() != 0);
		flags ^= SQF_FORCEPASSWORD;
	}
	else
	{
		setLocked(false);
	}

	if((flags & SQF_FORCEJOINPASSWORD) == SQF_FORCEJOINPASSWORD)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		in.skipRawData(1);
		flags ^= SQF_FORCEJOINPASSWORD;
	}

	if((flags & SQF_GAMESKILL) == SQF_GAMESKILL)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		setSkill(in.readQInt8());
		flags ^= SQF_GAMESKILL;
	}

	if((flags & SQF_BOTSKILL) == SQF_BOTSKILL)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		botSkill = in.readQInt8();
		flags ^= SQF_BOTSKILL;
	}

	if((flags & SQF_DMFLAGS) == SQF_DMFLAGS)
	{
		// Not supported because the amount of returned DMFlags
		// may vary depending on the server version. For example: old
		// servers may return only dmflags, dmflags2 and compatflags.
		// New servers may add dmflags3 and compatflags2.
		gLog << tr("Zandronum plugin: asking for DMFLAGS is not "
			"supported. This message should never appear.");
		//flags ^= SQF_DMFLAGS;

		//clearDMFlags();

		//const DMFlags& zandronumFlags = *ZandronumGameInfo::dmFlags();

		//// Read each dmflags section separately.
		//for (int i = 0; i < NUM_DMFLAG_SECTIONS; ++i)
		//{
		//	unsigned int dmflags = READINT32(&packetOut[pos]);
		//	pos += 4;

		//	const DMFlagsSection& zandronumFlagsSection = *zandronumFlags[i];
		//	DMFlagsSection* dmFlagsSection = new DMFlagsSection();
		//	dmFlagsSection->name = zandronumFlagsSection.name;

		//	// Iterate through every known flag to check whether it should be
		//	// inserted into the structure of this server.
		//	for (int j = 0; j < zandronumFlagsSection.flags.count(); ++j)
		//	{
		//		if ( (dmflags & (1 << zandronumFlagsSection.flags[j].value)) != 0)
		//		{
		//			dmFlagsSection->flags << zandronumFlagsSection.flags[j];
		//		}
		//	}

		//	dmFlags << dmFlagsSection;
		//}
	}

	if((flags & SQF_LIMITS) == SQF_LIMITS)
	{
		flags ^= SQF_LIMITS;

		RETURN_BAD_IF_NOT_ENOUGH_DATA(2);
		fragLimit = in.readQUInt16();

		// Read timelimit and timeleft,
		// note that if timelimit == 0 then no info
		// about timeleft is sent
		RETURN_BAD_IF_NOT_ENOUGH_DATA(2);
		setTimeLimit(in.readQUInt16());
		if (timeLimit() != 0)
		{
			RETURN_BAD_IF_NOT_ENOUGH_DATA(2);
			setTimeLeft(in.readQUInt16());
		}

		RETURN_BAD_IF_NOT_ENOUGH_DATA(2 + 2 + 2);
		duelLimit = in.readQUInt16();
		pointLimit = in.readQUInt16();
		winLimit = in.readQUInt16();
		switch(mode)
		{
			default:
				setScoreLimit(fragLimit);
				break;
			case ZandronumGameInfo::GAMEMODE_LASTMANSTANDING:
			case ZandronumGameInfo::GAMEMODE_TEAMLMS:
				setScoreLimit(winLimit);
				break;
			case ZandronumGameInfo::GAMEMODE_POSSESSION:
			case ZandronumGameInfo::GAMEMODE_TEAMPOSSESSION:
			case ZandronumGameInfo::GAMEMODE_TEAMGAME:
			case ZandronumGameInfo::GAMEMODE_CTF:
			case ZandronumGameInfo::GAMEMODE_ONEFLAGCTF:
			case ZandronumGameInfo::GAMEMODE_SKULLTAG:
			case ZandronumGameInfo::GAMEMODE_DOMINATION:
				setScoreLimit(pointLimit);
				break;
		}
	}
	else
	{
		// Nullify vars if there is no info
		fragLimit = 0;
		setTimeLimit(0);
		duelLimit = 0;
		pointLimit = 0;
		winLimit = 0;
		setScoreLimit(0);
	}

	if((flags & SQF_TEAMDAMAGE) == SQF_TEAMDAMAGE)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(4);
		teamDamage = in.readFloat();
		flags ^= SQF_TEAMDAMAGE;
	}
	if((flags & SQF_TEAMSCORES) == SQF_TEAMSCORES)
	{
		// DEPRECATED flag
		for(int i = 0;i < 2;i++)
		{
			RETURN_BAD_IF_NOT_ENOUGH_DATA(2);
			scoresMutable()[i] = in.readQInt16();
		}
		flags ^= SQF_TEAMSCORES;
	}
	if((flags & SQF_NUMPLAYERS) == SQF_NUMPLAYERS)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		int numPlayers = in.readQUInt8();
		flags ^= SQF_NUMPLAYERS;

		if((flags & SQF_PLAYERDATA) == SQF_PLAYERDATA)
		{
			flags ^= SQF_PLAYERDATA;
			clearPlayersList(); // Erase previous players (if any)
			for(int i = 0;i < numPlayers;i++)
			{
				// team isn't sent in non team modes.
				bool teammode = gameMode().isTeamGame();

				RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
				QString name = in.readRawUntilByte('\0');

				RETURN_BAD_IF_NOT_ENOUGH_DATA(2 + 2 + 1 + 1);
				int score = in.readQUInt16();
				int ping = in.readQUInt16();
				bool spectating = in.readQUInt8() != 0;
				bool bot = in.readQUInt8() != 0;

				int team;
				if (teammode)
				{
					RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
					team = in.readQUInt8();
				}
				else
				{
					team = Player::TEAM_NONE;
				}
				// Now there is info on time that the player is
				// on the server. We'll skip it.
				RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
				in.skipRawData(1);

				Player player(name, score, ping, static_cast<Player::PlayerTeam> (team), spectating, bot);
				addPlayer(player);
			}
		}
	}

	if((flags & SQF_TEAMINFO_NUMBER) == SQF_TEAMINFO_NUMBER)
	{
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		numTeams = in.readQUInt8();
		flags ^= SQF_TEAMINFO_NUMBER;
	}

	if((flags & SQF_TEAMINFO_NAME) == SQF_TEAMINFO_NAME)
	{
		flags ^= SQF_TEAMINFO_NAME;
		for(unsigned i = 0; i < numTeams && i < ST_MAX_TEAMS; ++i)
		{
			RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
			QString name = in.readRawUntilByte('\0');
			teamInfo[i].setName(tr(name.toAscii().constData()));
		}
	}
	if((flags & SQF_TEAMINFO_COLOR) == SQF_TEAMINFO_COLOR)
	{
		flags ^= SQF_TEAMINFO_COLOR;
		// NOTE: This may not be correct
		unsigned forLimit = qMin(numTeams, ST_MAX_TEAMS);

		for(unsigned i = 0; i < forLimit; i++)
		{
			RETURN_BAD_IF_NOT_ENOUGH_DATA(4);
			quint32 colorRgb = in.readQUInt32();
			teamInfo[i].setColor(QColor(colorRgb));
		}
	}
	if((flags & SQF_TEAMINFO_SCORE) == SQF_TEAMINFO_SCORE)
	{
		flags ^= SQF_TEAMINFO_SCORE;
		unsigned forLimit = qMin(numTeams, ST_MAX_TEAMS);

		for(unsigned i = 0; i < forLimit; i++)
		{
			RETURN_BAD_IF_NOT_ENOUGH_DATA(2);
			qint16 score = in.readQInt16();
			teamInfo[i].setScore(score);
			if(i < MAX_TEAMS) // Transfer to super class score array if possible.
			{
				scoresMutable()[i] = teamInfo[i].score();
			}
		}
	}

	// Due to a bug in 0.97d3 we need to add additional checks here.
	// 0.97d3 servers also respond with SQF_TESTING_SERVER flag set
	// if it was previously sent to them
	if (in.remaining() != 0 && ZandronumVersion(gameVersion()) > ZandronumVersion("0.97d3") && (flags & SQF_TESTING_SERVER) == SQF_TESTING_SERVER)
	{
		flags ^= SQF_TESTING_SERVER;

		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		testingServer = in.readQInt8() != 0;

		// '\0' is read if testingServer == false
		RETURN_BAD_IF_NOT_ENOUGH_DATA(1);
		testingArchive = in.readRawUntilByte('\0');
	}
	else
	{
		testingServer = false;
		testingArchive = QString();
	}

	if((flags & SQF_SECURITY_SETTINGS) == SQF_SECURITY_SETTINGS)
	{
		flags ^= SQF_SECURITY_SETTINGS;

		setSecure(in.readQUInt8() != 0);
	}

	return RESPONSE_GOOD;
}

QByteArray ZandronumServer::createSendRequest()
{
	// Prepare launcher challenge.
	int query = SQF_STANDARDQUERY;
	const unsigned char challenge[12] = {
		SERVER_CHALLENGE,
		WRITEINT32_DIRECT(unsigned char, query),
		WRITEINT32_DIRECT(unsigned char, millisecondTime())
	};
	char challengeOut[16];
	int out = 16;
	HUFFMAN_Encode(challenge, reinterpret_cast<unsigned char*> (challengeOut), 12, &out);
	QByteArray data(challengeOut, out);
	return data;
}

QRgb ZandronumServer::teamColor(unsigned team) const
{
	if(team >= ST_MAX_TEAMS)
		return Server::teamColor(team);

	return teamInfo[team].color().rgb();
}

QString	ZandronumServer::teamName(unsigned team) const
{
	if (team == 255)
		return "NO TEAM";

	return team < ST_MAX_TEAMS ? teamInfo[team].name() : "";
}

void ZandronumServer::updatedSlot(ServerPtr server, int response)
{
	if (response == RESPONSE_BAD)
	{
		// If response is bad we will print the read request to stderr,
		// for debug purposes of course.
		QSharedPointer<ZandronumServer> s = server.staticCast<ZandronumServer>();
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

ZandronumRConProtocol::ZandronumRConProtocol(ServerPtr server)
: RConProtocol(server)
{
	set_disconnectFromServer(&ZandronumRConProtocol::disconnectFromServer);
	set_sendCommand(&ZandronumRConProtocol::sendCommand);
	set_sendPassword(&ZandronumRConProtocol::sendPassword);
	set_packetReady(&ZandronumRConProtocol::packetReady);

	// Note: the original rcon utility did TIMEOUT/4.
	// Try to get at least 4 packets in before timing out,
	pingTimer.setInterval(2500);
	connect(&pingTimer, SIGNAL( timeout() ), this, SLOT( sendPong() ));
}

RConProtocol *ZandronumRConProtocol::connectToServer(ServerPtr server)
{
	ZandronumRConProtocol *protocol = new ZandronumRConProtocol(server);

	const unsigned char beginConnection[2] = { CLRC_BEGINCONNECTION, RCON_PROTOCOL_VERSION };
	char encodedConnection[4];
	int encodedSize = 4;
	HUFFMAN_Encode(beginConnection, reinterpret_cast<unsigned char*> (encodedConnection), 2, &encodedSize);

	// Try to connect, up to 3 times
	protocol->setConnected(false);
	for(unsigned int attempts = 0;attempts < 3;attempts++)
	{
		protocol->socket().writeDatagram(encodedConnection, encodedSize, protocol->address(), protocol->port());
		if(protocol->socket().waitForReadyRead(3000))
		{
			int size = protocol->socket().pendingDatagramSize();
			if (size <= 0)
			{
				// [Zalewa]
				// This situation always occurs when trying to connect to
				// a non-existent localhost server on Windows. I don't know
				// if it happens in any other situation.
				continue;
			}
			char* data = new char[size];
			protocol->socket().readDatagram(data, size);
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
					protocol->setConnected(true);
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

void ZandronumRConProtocol::disconnectFromServer()
{
	const unsigned char disconnectPacket[1] = { CLRC_DISCONNECT };
	char encodedDisconnect[4];
	int encodedSize = 4;
	HUFFMAN_Encode(disconnectPacket, reinterpret_cast<unsigned char*> (encodedDisconnect), 1, &encodedSize);
	socket().writeDatagram(encodedDisconnect, encodedSize, address(), port());
	setConnected(false);
	pingTimer.stop();
	emit disconnected();
}

void ZandronumRConProtocol::sendCommand(const QString &cmd)
{
	unsigned char packet[4096];
	packet[0] = CLRC_COMMAND;
	packet[cmd.length()+1] = 0;
	memcpy(packet+1, cmd.toAscii().constData(), cmd.length());
	char encodedPacket[4097];
	int encodedSize = 4097;
	HUFFMAN_Encode(packet, reinterpret_cast<unsigned char*> (encodedPacket), cmd.length()+2, &encodedSize);
	socket().writeDatagram(encodedPacket, encodedSize, address(), port());
}

void ZandronumRConProtocol::sendPassword(const QString &password)
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
		socket().writeDatagram(encodedPassword, encodedLength, address(), port());

		if(socket().waitForReadyRead(3000))
		{
			packetReady();
			connect(&socket(), SIGNAL( readyRead() ), this, SLOT( packetReady() ));
			break;
		}
	}
}

void ZandronumRConProtocol::sendPong()
{
	// create a "PONG" packet
	const unsigned char pong[1] = { CLRC_PONG };
	char encodedPong[4];
	int encodedSize = 4;
	HUFFMAN_Encode(pong, reinterpret_cast<unsigned char*> (encodedPong), 1, &encodedSize);
	socket().writeDatagram(encodedPong, encodedSize, address(), port());
}

void ZandronumRConProtocol::packetReady()
{
	if(!isConnected())
		return;

	while(socket().hasPendingDatagrams())
	{
		int size = socket().pendingDatagramSize();
		char* data = new char[size];
		socket().readDatagram(data, size);
		int decodedSize = 4096 + size;
		char* packet = new char[decodedSize];
		HUFFMAN_Decode(reinterpret_cast<const unsigned char*> (data), reinterpret_cast<unsigned char*> (packet), size, &decodedSize);
		delete[] data;

		QByteArray packetByteArray(packet, decodedSize);
		delete[] packet;

		QBuffer stream(&packetByteArray);
		stream.open(QIODevice::ReadOnly);
		processPacket(&stream);
	}
}

void ZandronumRConProtocol::processPacket(QIODevice* ioDevice, bool initial, int maxUpdates)
{
	static const QRegExp colorCode("\\\\c(\\[[a-zA-Z0-9]*\\]|[a-v+\\-!*])");

	// QIODevice is assumed to be already opened at this point.
	QDataStream dataStream(ioDevice);
	dataStream.setByteOrder(QDataStream::LittleEndian);
	DataStreamOperatorWrapper in(&dataStream);

	while(in.hasRemaining() && maxUpdates-- != 0)
	{
		// Determine how we get to the update.
		int update = 0;
		if(initial)
			update = SVRC_UPDATE;
		else
			update = in.readQUInt8();

		switch(update)
		{
			default:
				qDebug() << "Unknown update (" << update << ")";
				return;
			case SVRC_INVALIDPASSWORD:
				emit invalidPassword();
				break;
			case SVRC_LOGGEDIN:
			{
				connect(&socket(), SIGNAL( readyRead() ), this, SLOT( packetReady() ));
				serverProtocolVersion = in.readQUInt8();
				hostName = in.readRawUntilByte('\0');
				emit serverNameChanged(hostName);

				int numUpdates = in.readQUInt8();

				processPacket(ioDevice, true, numUpdates);

				int numStrings = in.readQUInt8();
				while(numStrings-- > 0)
				{
					QString message = in.readRawUntilByte('\0');
					message.replace(colorCode, "\034\\1");
					emit messageReceived(message.trimmed());
				}
				break;
			}
			case SVRC_MESSAGE:
			{
				QString message = QDateTime::currentDateTime().toString("[hh:mm:ss ap] ") + in.readRawUntilByte('\0');
				message.replace(colorCode, "\034\\1");
				emit messageReceived(message);
				break;
			}
			case SVRC_UPDATE:
				int updateType = in.readQUInt8();
				switch(updateType)
				{
					default:
						qDebug() << "Uknown streamlined update (" << update << ")";
						return;
					case SVRCU_MAP:
					{
						QString map = in.readRawUntilByte('\0');
						break;
					}
					case SVRCU_ADMINCOUNT:
					{
						// Unused:
						// int admins = in.readQUint8();
						// !!! MAKE SURE to remove the line below if line above
						// is uncommented!
						in.skipRawData(1);
						break;
					}
					case SVRCU_PLAYERDATA:
					{
						int players = in.readQUInt8();
						this->playersMutable().clear();
						while(players-- > 0)
						{
							QString player = in.readRawUntilByte('\0');
							this->playersMutable().append(Player(player, 0, 0));
						}
						emit playerListUpdated();
						break;
					}
				}
				break;
		}
	}
}

RConProtocol *ZandronumServer::rcon()
{
	return ZandronumRConProtocol::connectToServer(self());
}
