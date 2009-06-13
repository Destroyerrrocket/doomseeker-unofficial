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

////////////////////////////////////////////////////////////////////////////////

// I really don't know if the game modes will need to be translated but I
// wrapped them in tr() just to be safe.
const GameMode GameMode::COOPERATIVE(QObject::tr("Cooperative"), false);
const GameMode GameMode::DEATHMATCH(QObject::tr("Deathmatch"), false);
const GameMode GameMode::TEAM_DEATHMATCH(QObject::tr("Team DM"), true);
const GameMode GameMode::CAPTURE_THE_FLAG(QObject::tr("CTF"), true);

GameMode::GameMode(const QString &name, bool teamgame) : modeName(name), teamgame(teamgame)
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
	bDelete = false;
	bKnown = false;
	bRunning = false;
	custom = false;
	for(int i = 0;i < MAX_TEAMS;i++)
		scores[i] = 0;

	connect(this, SIGNAL( updated(Server*, int) ), this, SLOT( setResponse(Server*, int) ));
}

Server::Server(const Server &other) : QObject(), currentGameMode(GameMode::COOPERATIVE)
{
	(*this) = other;
	connect(this, SIGNAL( updated(Server*, int) ), this, SLOT( setResponse(Server* server, int response) ));
}

Server::~Server()
{
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

QString Server::clientBinarysDirectoy()
{
	QFileInfo fi(clientBinary());
	return fi.canonicalPath();
}

void Server::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const
{
	// Connect
	args << "+connect" << QString(address().toString() + ":" + QString::number(port()));
	if(isLocked())
		args << connectPassword;

	// Iwad
	QString iwad = pf.findWad(iwadName().toLower());
	args << "-iwad" << iwad;
	iwadFound = !iwad.isEmpty();
}

bool Server::createJoinCommandLine(QFileInfo& executablePath, QDir& applicationDir, QStringList& args, const QString &connectPassword) const
{
	const QString errorCaption = tr("Doomseeker - error");
	args.clear();

	QString clientBin = clientBinary();
	if (clientBin.isEmpty())
	{
		QMessageBox::critical(NULL, errorCaption, tr("No executable specified for this engine."));
		return false;
	}

	executablePath = QFileInfo(clientBin);

	if (!executablePath.exists() || executablePath.isDir())
	{
		QMessageBox::critical(NULL, errorCaption, tr("File: ") + executablePath.absoluteFilePath() + tr("\ndoesn't exist or is a directory"));
		return false;
	}

	PathFinder pf(Main::config);
	QStringList missingPwads;
	bool iwadFound = false;

	connectParameters(args, pf, iwadFound, connectPassword);

	// Pwads
	if (numWads() != 0)
	{
		args << "-file";
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
			args << pwad;
		}
	}

	if (!iwadFound || !missingPwads.isEmpty())
	{
		const QString filesMissingCaption = tr("Doomseeker - files are missing");
		QString error = tr("Following files are missing:\n");

		if (!iwadFound)
		{
			error += tr("IWAD: ") + iwadName().toLower() + "\n";
		}

		if (!missingPwads.isEmpty())
		{
			error += tr("PWADS: %1\nDo you want Wadseeker to find missing PWADS?").arg(missingPwads.join(" "));
		}

		if (QMessageBox::question(NULL, filesMissingCaption, error, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
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
				return createJoinCommandLine(executablePath, applicationDir, args, connectPassword);
			}
		}

		return false;
	}

	return true;
}

void Server::displayJoinCommandLine()
{
	QFileInfo executablePath;
	QDir applicationDir;
	QStringList args;
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
	if (!createJoinCommandLine(executablePath, applicationDir, args, connectPassword))
		return;

	CopyTextDlg* ctd = new CopyTextDlg(executablePath.absoluteFilePath() + " " + args.join(" "), "Join command line:", Main::mainWindow);
	ctd->show();
}

void Server::join(const QString &connectPassword) const
{
	const QString errorCaption = tr("Doomseeker - error");
	QFileInfo fileinfo;
	QDir applicationDir;
	QStringList args;

	if (!createJoinCommandLine(fileinfo, applicationDir, args, connectPassword))
		return;

	printf("Starting: %s %s\n", fileinfo.absoluteFilePath().toAscii().constData(), args.join(" ").toAscii().constData());

	QProcess proc;
	if( !proc.startDetached(fileinfo.absoluteFilePath(), args, fileinfo.absolutePath()) )
	{
		QMessageBox::critical(NULL, errorCaption, tr("File: ") + fileinfo.absoluteFilePath() + tr("\ncannot be run"));
		return;
	}
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

////////////////////////////////////////////////////////////////////////////////

QThreadPool ServerRefresher::threadPool;
QMutex ServerRefresher::guardianMutex;
bool ServerRefresher::bGuardianExists = false;

void Server::doRefresh(bool& bKillThread)
{
	bKillThread = false;
	// Connect to the server
	QUdpSocket socket;

	socket.connectToHost(address(), port());

	if(!socket.waitForConnected(1000))
	{
		printf("%s\n", socket.errorString().toAscii().data());
		emitUpdated(Server::RESPONSE_BAD);
		return;
	}

	QByteArray request;
	if(!sendRequest(request))
		return;

	// start timer and write.
	QTime time = QTime::currentTime();
	socket.write(request);
	time.start();
	int queryTimeout = Main::config->setting("QueryTimeout")->integer();

	bool bOk = socket.waitForReadyRead(queryTimeout);

	if (!Main::running)
	{
		bKillThread = true;
		return;
	}

	if (bDelete)
	{
		bKillThread = true;
		delete this;
		return;
	}

	if(!bOk)
	{
		emitUpdated(Server::RESPONSE_TIMEOUT);
		return;
	}

	// Read
	QByteArray data = socket.readAll();
	currentPing = time.elapsed();
	if(!readRequest(data))
		return;

	socket.close();

	emitUpdated(Server::RESPONSE_GOOD);
}

void Server::refresh()
{
	if (bRunning)
		return;

	startRunning();
	emit begunRefreshing(this);
	ServerRefresher* r = new ServerRefresher(this);
	ServerRefresher::threadPool.start(r);
}

void Server::finalizeRefreshing()
{
	iwad = iwad.toLower();
}

void Server::setToDelete(bool b)
{
	bDelete = b;
	if (!bRunning)
		delete this;
}

ServerRefresher::ServerRefresher(Server* p) : parent(p)
{
	bGuardian = false;
	int queryThreads = Main::config->setting("QueryThreads")->integer();
	if(threadPool.maxThreadCount() != queryThreads)
	{
		threadPool.setMaxThreadCount(queryThreads);
	}
}

void ServerRefresher::startGuardian()
{
	guardianMutex.lock();
	if (!bGuardianExists)
	{
		bGuardianExists = true;
		bGuardian = true;
		QThreadPool::globalInstance()->start(this);
	}
	guardianMutex.unlock();
}

void ServerRefresher::run()
{
	if (!bGuardian)
	{
		// If the program is no longer running then do nothing.
		bool bKillThread = false;
		if(Main::running)
			parent->doRefresh(bKillThread);

		if (bKillThread)
			return;

		if (Main::running)
			parent->finalizeRefreshing();

		if (Main::running)
			parent->stopRunning();

		if (Main::running && parent->isSetToDelete())
			delete parent;
	}
	else
	{
		threadPool.waitForDone();
		if (Main::running)
		{
			emit allServersRefreshed();
			bGuardianExists = false;
		}
	}
}
