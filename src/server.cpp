//------------------------------------------------------------------------------
// server.cpp
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

#include "server.h"
#include "main.h"
#include "gui/copytextdlg.h"
#include "gui/passwordDlg.h"
#include "gui/standardserverconsole.h"
#include "gui/wadseekerinterface.h"
#include <QMessageBox>
#include <QProcess>
#include <QSet>
#include <QTime>
#include <QUdpSocket>

// \c = '\034'
#define	ESCAPE_COLOR	'\034'

Player::Player(const QString &name, unsigned short score, unsigned short ping, PlayerTeam team, bool spectator, bool bot) :
	playerName(name), currentScore(score), currentPing(ping), team(team), spectator(spectator), bot(bot)
{
}

const char Player::colorChart[20][7] =
{
	"FF91A4", //a
	"D2B48C", //b
	"808080", //c
	"32CD32", //d
	"918151", //e
	"F4C430", //f
	"E32636", //g
	"0000FF", //h
	"FF8C00", //i
	"C0C0C0", //j
	"FFD700", //k
	"E34234", //l
	"000000", //m
	"4169E1", //n
	"FFDEAD", //o
	"465945", //p
	"228b22", //q
	"800000", //r
	"704214", //s
	"A020F0", //t
};
QString Player::colorizeString(const QString &str, int current)
{
	QString ret;
	bool colored = false;
	for(int i = 0;i < str.length();i++)
	{
		if(str[i] == ESCAPE_COLOR)
		{
			i++;
			if(i >= str.length())
				break;
			QChar colorChar = str[i].toLower();
			int color = colorChar.toAscii() - 97;

			// special cases
			if(colorChar == '+')
				color = current == 0 ? 19 : current-1; // + is the current minus one, wrap if needed.
			else if(colorChar == '*')
				color = 3; // Chat color which is usally green
			else if(colorChar == '!')
				color = 16; // Team char (usually green, but made dark green here for distinction)
			else if(colorChar == '-' || colored)
			{
				if(colored)
				{
					ret += "</span>";
				}
				continue;
			}

			if(color >= 0 && color < 20)
			{
				ret += QString("<span style=\"color: #") + colorChart[color] + "\">";
			}
			continue;
		}
		ret += str[i];
	}
	if(colored)
		ret += "</span>";
	return ret;
}

QString Player::nameColorTagsStripped() const
{
	QString ret;
	for (int i = 0; i < playerName.length(); ++i)
	{
		if (playerName[i] < 32 || playerName[i] > 126)
		{
			// Lets only remove the following character on \c.
			// Removing the control characters is still a good idea though.
			if(playerName[i] == ESCAPE_COLOR)
				++i;
			continue;
		}

		ret += playerName[i];
	}
	return ret;
}

QString	Player::nameFormatted() const
{
	QString ret;
	for (int i = 0; i < playerName.length(); ++i)
	{
		// cut out bad characters
		if ((playerName[i] < 32 || playerName[i] > 126) && playerName[i] != ESCAPE_COLOR)
			continue;

		switch (playerName[i].toAscii())
		{
			case '<':
				ret += "&lt;";
				break;

			case '>':
				ret += "&gt;";
				break;

			default:
				ret += playerName[i];
				break;
		}
	}

	return colorizeString(ret);
}

////////////////////////////////////////////////////////////////////////////////

// I really don't know if the game modes will need to be translated but I
// wrapped them in tr() just to be safe.
const GameMode GameMode::COOPERATIVE(SGMICooperative, QObject::tr("Cooperative"), false);
const GameMode GameMode::DEATHMATCH(SGMIDeathmatch, QObject::tr("Deathmatch"), false);
const GameMode GameMode::TEAM_DEATHMATCH(SGMITeamDeathmatch, QObject::tr("Team DM"), true);
const GameMode GameMode::CAPTURE_THE_FLAG(SGMICTF, QObject::tr("CTF"), true);

GameMode::GameMode(int index, const QString &name, bool teamgame) : gameModeIndex(index), modeName(name), teamgame(teamgame)
{
}

////////////////////////////////////////////////////////////////////////////////
const int SkillLevel::numSkillLevels = 5;

const QString SkillLevel::names[] =
{
	QObject::tr("1 - I'm too young to die."),
	QObject::tr("2 - Hey, not too rough."),
	QObject::tr("3 - Hurt me plenty."),
	QObject::tr("4 - Ultra-Violence."),
	QObject::tr("5 - NIGHTMARE!")
};

////////////////////////////////////////////////////////////////////////////////

QString Server::teamNames[] =
{
	"Blue",
	"Red",
	"Green",
	"Gold"
};

Server::Server(const QHostAddress &address, unsigned short port) : QObject(),
	serverAddress(address), serverPort(port),
	currentGameMode(GameMode::COOPERATIVE), currentPing(999), locked(false),
	maxClients(0), maxPlayers(0), serverName(tr("<< ERROR >>")),
	serverScoreLimit(0), serverTimeLeft(0), serverTimeLimit(0)
{
	broadcastToLAN = false;
	broadcastToMaster = false;
	mapRandomRotation = false;
	bDelete = false;
	bKnown = false;
	bIsRefreshing = false;
	custom = false;
	skill = 3;
	for(int i = 0;i < MAX_TEAMS;i++)
		scores[i] = 0;

	connect(this, SIGNAL( updated(Server*, int) ), this, SLOT( setResponse(Server*, int) ));
}

Server::Server(const Server &other) : QObject(), currentGameMode(GameMode::COOPERATIVE)
{
	(*this) = other;
	connect(this, SIGNAL( updated(Server*, int) ), this, SLOT( setResponse(Server*, int) ));
}

Server::~Server()
{
	clearDMFlags();
}

QList<ServerAction>* Server::actions()
{
	QList<ServerAction>* lst = new QList<ServerAction>();

	ServerAction sa;
	sa.label = "Join command line";
	sa.receiver = this;
	sa.slot = SLOT( displayJoinCommandLine() );

	lst->append(sa);

	actionsEx(lst);
	return lst;
}

void Server::cleanArguments(QStringList& args) const
{
	#ifdef Q_OS_WIN32
	QStringList::iterator it;
	for (it = args.begin(); it != args.end(); ++it)
	{
		if (it->contains(" "))
		{
			Main::trim(*it, "\"");
		}
	}
	#endif
}

void Server::clearDMFlags()
{
	DMFlagsIt it;
	for (it = dmFlags.begin(); it != dmFlags.end(); ++it)
	{
		delete (*it);
	}
	dmFlags.clear();
}

QString Server::clientWorkingDirectory() const
{
	QString dummy;
	QFileInfo fi(clientBinary(dummy));
	return fi.absolutePath();
}

void Server::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const
{
	// Connect
	args << "-connect" << QString(address().toString() + ":" + QString::number(port()));
	if(isLocked())
		args << connectPassword;

	// Iwad
	QString iwad = pf.findWad(iwadName().toLower());
	args << argForIwadLoading() << iwad;
	iwadFound = !iwad.isEmpty();
}

bool Server::createHostCommandLine(const HostInfo& hostInfo, CommandLineInfo& cli, QString& error) const
{
	QDir& applicationDir = cli.applicationDir;
	QFileInfo& executablePath = cli.executable;
	QStringList& args = cli.args;

	const QString errorCaption = tr("Doomseeker - error");
	args.clear();

	// First some wad path checks, add wad paths to the args if check passes:
	const QString& iwadPath = hostInfo.iwadPath;
	if (iwadPath.isEmpty())
	{
		error = tr("Iwad is not set");
		return false;
	}

	QFileInfo fi(iwadPath);

	if (!fi.isFile())
	{
		error = tr("Iwad Path error:\n\"%1\" doesn't exist or is a directory!").arg(iwadPath);
		return false;
	}

	args << argForIwadLoading() << iwadPath;

	const QStringList& pwadsPaths = hostInfo.pwadsPaths;
	if (!pwadsPaths.isEmpty())
	{
		args << argForPwadLoading();

		foreach(const QString s, pwadsPaths)
		{
			fi.setFile(s);
			if (!fi.isFile())
			{
				error = tr("Pwad path error:\n\"%1\" doesn't exist or is a directory!").arg(s);
				return false;
			}
			args << s;
		}
	}
	// Checks done.

	// Port
	args << argForPort() << QString::number(serverPort);

	// CVars
	const QList<GameCVar>& cvars = hostInfo.cvars;
	foreach(const GameCVar c, cvars)
	{
		args << QString("+" + c.consoleCommand) << c.value();
	}

	const QString& serverExecutablePath = hostInfo.executablePath;
	if (serverExecutablePath.isEmpty())
	{
		QString serverBin = serverBinary(error);
		if (serverBin.isEmpty())
		{
			return false;
		}
		executablePath = serverBin;
	}
	else
	{
		executablePath = serverExecutablePath;
	}

	if (!executablePath.isFile())
	{
		error = tr("%1\n doesn't exist or is not a file.").arg(executablePath.filePath());
		return false;
	}

	QString serverWorkingDirPath = serverWorkingDirectory();
	applicationDir = serverWorkingDirectory();

	if (serverWorkingDirPath.isEmpty())
	{
		error = tr("Path to working directory is empty.\nMake sure the configuration for the main binary is set properly.");
		return false;
	}
	else if (!applicationDir.exists())
	{
		error = tr("%1\n cannot be used as working directory for:\n%2").arg(serverWorkingDirPath, executablePath.filePath());
		return false;
	}

	args << argForServerLaunch();
	hostDMFlags(args, dmFlags);
	hostProperties(args);
	args.append(hostInfo.customParameters);

	return true;
}

bool Server::createJoinCommandLine(CommandLineInfo& cli, const QString &connectPassword, QString& error) const
{
	QDir& applicationDir = cli.applicationDir;
	QFileInfo& executablePath = cli.executable;
	QStringList& args = cli.args;

	const QString errorCaption = tr("Doomseeker - error");
	args.clear();

	QString clientBin = clientBinary(error);
	if (clientBin.isEmpty())
	{
		return false;
	}

	executablePath = clientBin;

	QString clientWorkingDirPath = clientWorkingDirectory();
	applicationDir = clientWorkingDirPath;

	if (clientWorkingDirPath.isEmpty())
	{
		error = tr("Path to working directory is empty.\nMake sure the configuration for the main binary is set properly.");
		return false;
	}
	else if (!applicationDir.exists())
	{
		error = tr("%1\n cannot be used as working directory for:\n%2").arg(clientWorkingDirPath, clientBin);
		return false;
	}

	PathFinder pf(Main::config);
	QStringList missingPwads;
	bool iwadFound = false;

	connectParameters(cli.args, pf, iwadFound, connectPassword);

	// Pwads
	if (numWads() != 0)
	{
		cli.args << argForPwadLoading();
	}

	for (int i = 0; i < numWads(); ++i)
	{
		QString pwad = pf.findWad(wad(i));
		if (pwad.isEmpty())
		{
			missingPwads << wad(i);
		}
		else
		{
			cli.args << pwad;
		}
	}

	if (!iwadFound || !missingPwads.isEmpty())
	{
		const QString filesMissingCaption = tr("Doomseeker - files are missing");
		QString filesMissingMessage = tr("Following files are missing:\n");

		if (!iwadFound)
		{
			filesMissingMessage += tr("IWAD: ") + iwadName().toLower() + "\n";
		}

		if (!missingPwads.isEmpty())
		{
			filesMissingMessage += tr("PWADS: %1\nDo you want Wadseeker to find missing PWADS?").arg(missingPwads.join(" "));
		}

		if (QMessageBox::question(Main::mainWindow, filesMissingCaption, filesMissingMessage, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			if (!iwadFound)
			{
				missingPwads.append(iwadName());
			}

			WadSeekerInterface wsi;
			wsi.setAutomatic(true, missingPwads);
			wsi.wadseekerRef().setCustomSite(website());
			if (wsi.exec() == QDialog::Accepted)
			{
				return createJoinCommandLine(cli, connectPassword, error);
			}
		}

		return false;
	}

	return true;
}

void Server::displayJoinCommandLine()
{
	CommandLineInfo cli;

	QString connectPassword;
	if(isLocked())
	{
		PasswordDlg password;
		int ret = password.exec();
		if(ret == QDialog::Accepted)
			connectPassword = password.connectPassword();
		else
			return;
	}

	QString error; // TODO: proper implementation
	if (!createJoinCommandLine(cli, connectPassword, error))
	{
		return;
	}

	CopyTextDlg* ctd = new CopyTextDlg(cli.executable.absoluteFilePath() + " " + cli.args.join(" "), "Join command line:", Main::mainWindow);
	ctd->show();
}

QString Server::gameInfoTableHTML() const
{
	const QString timelimit = tr("Timelimit");
	const QString scorelimit = tr("Scorelimit");
	const QString unlimited = tr("Unlimited");
	const QString players = tr("Players");

	// Timelimit
    QString firstTableTimelimit = "<tr><td>" + timelimit + ":&nbsp;</td><td>%1 %2</td></tr>";
    if (this->timeLimit() == 0)
    {
    	firstTableTimelimit = firstTableTimelimit.arg(unlimited, "");
    }
    else
    {
		QString strLeft = tr("(%1 left)").arg(this->timeLeft());
		firstTableTimelimit = firstTableTimelimit.arg(this->timeLimit()).arg(strLeft);
    }

	// Scorelimit
	QString firstTableScorelimit = "<tr><td>" + scorelimit + ":&nbsp;</td><td>%1</td></tr>";
	if (this->scoreLimit() == 0)
	{
		firstTableScorelimit = firstTableScorelimit.arg(unlimited);
	}
	else
	{
		firstTableScorelimit = firstTableScorelimit.arg(this->scoreLimit());
	}

	// Team score
	QString firstTableTeamscore;
	if (this->gameMode().isTeamGame())
	{
		firstTableTeamscore = "<tr><td colspan=\"2\">%1</td></tr>";
		QString teams;
		bool bPrependBar = false;
		for (int i = 0; i < MAX_TEAMS; ++i)
		{
			if (this->teamPlayerCount(i) != 0)
			{
				if (bPrependBar)
					teams += " | ";
				teams += teamName(i) + ": " + QString::number(this->score(i));
				bPrependBar = true;
			}
		}
		firstTableTeamscore = firstTableTeamscore.arg(teams);
	}

	// Players
	QString firstTablePlayers = "<tr><td>" + players + ":&nbsp;</td><td>%1 / %2</td></tr>";
	firstTablePlayers = firstTablePlayers.arg(this->numPlayers()).arg(this->maximumClients());

	QString firstTable = "<table>";
	firstTable += firstTableTimelimit;
	firstTable += firstTableScorelimit;
	firstTable += firstTableTeamscore;
	firstTable += firstTablePlayers;
	firstTable += "</table>";

	return firstTable;
}

QString Server::generalInfoHTML() const
{
	QString ret = QString(name()).replace('>', "&gt;").replace('<', "&lt;") + "\n";
	if(!version().isEmpty())
	{
		ret += tr("Version") + ": " + version() + "\n";
	}
	if (!email.isEmpty())
	{
		ret += tr("E-mail") + ": " + email + "\n";
	}
	if (!webSite.isEmpty())
	{
		ret += tr("URL") + ": <a href=\"" + webSite + "\">" + webSite + "</a>\n";
	}
	return ret;
}

bool Server::host(const HostInfo& hostInfo, QString& error)
{
	error.clear();
	CommandLineInfo cli;

	if (!createHostCommandLine(hostInfo, cli, error))
		return false;

#ifdef Q_OS_WIN32
	const bool WRAP_IN_SSS_CONSOLE = false;
#else
	const bool WRAP_IN_SSS_CONSOLE = true;
#endif

	return runExecutable(cli, WRAP_IN_SSS_CONSOLE, error);
}

bool Server::join(QString& error, const QString &connectPassword) const
{
	CommandLineInfo cli;

	if (!createJoinCommandLine(cli, connectPassword, error))
	{
		return false;
	}

	return runExecutable(cli, false, error);
}

unsigned int Server::longestPlayerName() const
{
	unsigned int x = 0;
	for (int i = 0; i < numPlayers(); ++i)
	{
		unsigned int len = players[i].nameColorTagsStripped().length();
		if (len > x)
			x = len;
	}
	return x;
}

QList<ServerInfo>* Server::serverInfo() const
{
	QList<ServerInfo>* list = new QList<ServerInfo>();

	ServerInfo siName = { this->name(), "<div style='white-space: pre'>" + this->name() + "</div>" };

	QString addr = this->address().toString() + ":" + QString::number(this->port());
	ServerInfo siAddress = { addr, addr };

	list->append(siName);
	list->append(siAddress);

	if (!this->webSite.isEmpty())
	{
		QString url = "<A HREF=\"" + this->webSite + "\">" + this->webSite + "</A>";
		ServerInfo siUrl = { tr("URL: ") + url, "<div style='white-space: pre'>" + this->webSite + "</div>"};
		list->append(siUrl);
	}
	if (!this->email.isEmpty())
	{
		QString email = "<a href=\"mailto:" + this->email + "\">" + this->email + "</a>";
		ServerInfo siEmail = { tr("E-mail: ") + email, "<div style='white-space: pre'>" + this->email + "</div>"};
		list->append(siEmail);
	}

	additionalServerInfo(list);
	return list;
}

QString Server::playerTableHTML() const
{
	// Sort the players out first.
	QHash<int, QList<const Player*> > sortedPlayers;
	QList<const Player*> botList;
	QList<const Player*> specList;

	for (int i = 0; i < this->numPlayers(); ++i)
	{
		const Player& p = this->player(i);

		if (p.isSpectating())
		{
			specList.append(&p);
			continue;
		}

		if (this->gameMode().isTeamGame())
		{
			int team = p.teamNum();

			QHash<int, QList<const Player*> >::iterator it = sortedPlayers.find(team);
			if (it == sortedPlayers.end())
			{
				QList<const Player*> l;
				l.append(&p);
				sortedPlayers.insert(team, l);
			}
			else
			{
				it.value().append(&p);
			}
		}
		else
		{
			if (p.isBot())
			{
				botList.append(&p);
				continue;
			}

			if (sortedPlayers.count() == 0)
				sortedPlayers.insert(0, QList<const Player*>());

			sortedPlayers.find(0).value().append(&p);
		}

	}

	const QString team = tr("Team");
	const QString player = tr("Player");
	const QString score = tr("Score");
	const QString ping = tr("Ping");
	const QString status = tr("Status");

	QString plTabTeamHeader;
	QString plTabHeader = "<tr>";
	int plTabColNum = 4;
	if (this->gameMode().isTeamGame())
	{
		plTabColNum = 5;
		plTabTeamHeader = "<td>" + team + "</td>";
	}
	plTabHeader += plTabTeamHeader + "<td>" + player + "</td><td align=\"right\">&nbsp;" + score + "</td><td align=\"right\">&nbsp;" + ping + "</td><td>" + status + "</td></tr>";
	plTabHeader += QString("<tr><td colspan=\"%1\"><hr width=\"100%\"></td></tr>").arg(plTabColNum);

	QString plTabPlayers;

	QList<int> hashKeys = sortedPlayers.uniqueKeys();
	qSort(hashKeys);
	QList<int>::iterator keyit;

	bool bAppendEmptyRowAtBeginning = false;
	for (keyit = hashKeys.begin(); keyit != hashKeys.end(); ++keyit)
	{
		plTabPlayers += spawnPartOfPlayerTable(sortedPlayers[*keyit], plTabColNum, bAppendEmptyRowAtBeginning);
		if (!bAppendEmptyRowAtBeginning)
		{
			bAppendEmptyRowAtBeginning = true;
		}
	}

	bAppendEmptyRowAtBeginning = !plTabPlayers.isEmpty();
	QString plTabBots = spawnPartOfPlayerTable(botList, plTabColNum, bAppendEmptyRowAtBeginning);

	bAppendEmptyRowAtBeginning = !(plTabBots.isEmpty() && plTabPlayers.isEmpty());
	QString plTabSpecs = spawnPartOfPlayerTable(specList, plTabColNum, bAppendEmptyRowAtBeginning);


	QString plTab = "<table cellspacing=\"4\" style=\"background-color: #FFFFFF;color: #000000\">";
	plTab += plTabHeader;
	plTab += plTabPlayers;
	plTab += plTabBots;
	plTab += plTabSpecs;
	plTab += "</table>";
	return plTab;
}

bool Server::refresh()
{
	if (Main::refreshingThread == NULL)
	{
		emitUpdated(RESPONSE_BAD);
		printf("REFRESHING THREAD IS NULL\n");
		return false;
	}

	Main::refreshingThread->registerServer(this);
	return true;
}

void Server::refreshStarts()
{
	bIsRefreshing = true;

	emit begunRefreshing(this);
	triesLeft = Main::config->setting("QueryTries")->integer();
	if(triesLeft > 10) // Limit the maximum number of tries
		triesLeft = 10;
}

void Server::refreshStops()
{
	bIsRefreshing = false;
	iwad = iwad.toLower();
}

bool Server::runExecutable(const CommandLineInfo& cli, bool bWrapInStandardServerConsole, QString& error) const
{
	printf("Starting (working dir %s): %s %s\n", cli.applicationDir.canonicalPath().toAscii().constData(), cli.executable.absoluteFilePath().toAscii().constData(), cli.args.join(" ").toAscii().constData());
	QStringList args = cli.args;
	cleanArguments(args);

	if (!bWrapInStandardServerConsole)
	{
		if( !QProcess::startDetached(cli.executable.canonicalFilePath(), args, cli.applicationDir.canonicalPath()) )
		{
			error = tr("File: %1\ncannot be run").arg(cli.executable.canonicalFilePath());
			return false;
		}
	}
	else
	{
		new StandardServerConsole(this, cli.executable.canonicalFilePath(), args);
	}

	return true;
}

bool Server::sendRefreshQuery(QUdpSocket* socket)
{
	if(triesLeft-- == 0)
	{
		emitUpdated(Server::RESPONSE_TIMEOUT);
		refreshStops();
		return false;
	}

	QByteArray request;
	if (!sendRequest(request))
	{
		emitUpdated(Server::RESPONSE_BAD);
		refreshStops();
		return false;
	}

	time.start();

	socket->writeDatagram(request, address(), port());

	return true;
}

void Server::setResponse(Server* server, int res)
{
	response = static_cast<Response>(res);
	if (response == RESPONSE_GOOD)
	{
		bKnown = true;
	}
	else if (response == RESPONSE_BAD || response == RESPONSE_BANNED || response == RESPONSE_TIMEOUT)
	{
		bKnown = false;
	}

}

void Server::setToDelete(bool b)
{
	bDelete = b;
	if (!bIsRefreshing)
		delete this;
}

int Server::teamPlayerCount(int team) const
{
	if (team >= MAX_TEAMS)
	{
		return -1;
	}

	int teamSize = 0;
	for (int i = 0; i < players.count(); ++i)
	{
		const Player& p = players[i];
		if (p.teamNum() == team)
		{
			++teamSize;
		}
	}
	return teamSize;
}


QString Server::serverWorkingDirectory() const
{
	QString dummy;
	QFileInfo fi(serverBinary(dummy));
	return fi.absolutePath();
}

QString Server::spawnPartOfPlayerTable(QList<const Player*> list, int colspan, bool bAppendEmptyRowAtBeginning) const
{
	QString ret;
	if (list.count() != 0)
	{
		if (bAppendEmptyRowAtBeginning)
		{
			ret = QString("<tr><td colspan=\"%1\">&nbsp;</td></tr>").arg(colspan);
		}

		for (int i = 0; i < list.count(); ++i)
		{
			const Player& p = *list[i];

			QString status = "";
			if (p.isBot())
				status = tr("BOT");
			else if (p.isSpectating())
				status = tr("SPECTATOR");

			QString strPlayer = "<tr>";
			if (gameMode().isTeamGame())
			{
				strPlayer += QString("<td>%1</td>").arg(teamName(p.teamNum()));
			}
			strPlayer += "<td>%1</td><td align=\"right\">%2</td><td align=\"right\">%3</td><td>%4</td></tr>";
			strPlayer = strPlayer.arg(p.nameFormatted()).arg(p.score()).arg(p.ping());
			strPlayer = strPlayer.arg(status);

			ret += strPlayer;
		}
	}

	return ret;
}

QRgb Server::teamColor(int team) const
{
	switch(team)
	{
		case Player::TEAM_BLUE: return qRgb(0, 0, 255);
		case Player::TEAM_RED: return qRgb(255, 0, 0);
		case Player::TEAM_GREEN: return qRgb(0, 255, 0);
		case Player::TEAM_GOLD: return qRgb(255, 255, 0);
		default: break;
	}
	return qRgb(0, 255, 0);
}

void Server::operator= (const Server &other)
{
	serverAddress = other.address();
	serverPort = other.port();

	bDelete = other.bDelete;
	bKnown = other.isKnown();
	currentGameMode = other.gameMode();
	currentPing = other.ping();
	custom = other.isCustom();
	locked = other.isLocked();
	maxClients = other.maximumClients();
	maxPlayers = other.maximumPlayers();
	serverName = other.name();
	serverScoreLimit = other.scoreLimit();
}

////////////////////////////////////////////////////////////////////////////////

RConProtocol::RConProtocol(Server *server) : server(server)
{
	socket.bind();
}

RConProtocol::~RConProtocol()
{
	socket.close();
}
