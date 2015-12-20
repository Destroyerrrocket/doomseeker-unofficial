//------------------------------------------------------------------------------
// gamecreateparams.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "gamecreateparams.h"

#include "serverapi/serverstructs.h"
#include "gamedemo.h"
#include <QFileInfo>

DClass<GameCreateParams>
{
	public:
		bool broadcastToLan;
		bool broadcastToMaster;
		QString connectPassword;
		QStringList customParameters;
		QList<GameCVar> cvars;
		QString executablePath;
		QString demoPath;
		GameDemo demoRecord;
		QList<DMFlagsSection> dmFlags;
		QString email;
		GameMode gameMode;
		GameCreateParams::HostMode hostMode;
		QString ingamePassword;
		QString iwadPath;
		QString map;
		QStringList mapList;
		int maxClients;
		int maxPlayers;
		QString motd;
		QString name;
		unsigned short port;
		QList<bool> pwadsOptional;
		QStringList pwadsPaths;
		bool randomMapRotation;
		QString rconPassword;
		int skill;
		QString url;
};

DPointered(GameCreateParams)

GameCreateParams::GameCreateParams()
{
	d->broadcastToLan = true;
	d->broadcastToMaster = true;
	d->randomMapRotation = false;
	d->maxClients = 0;
	d->maxPlayers = 0;
	d->port = 0;
	d->skill = 0;
}

GameCreateParams::~GameCreateParams()
{
}

const QString& GameCreateParams::connectPassword() const
{
	return d->connectPassword;
}

QStringList& GameCreateParams::customParameters()
{
	return d->customParameters;
}

const QStringList& GameCreateParams::customParameters() const
{
	return d->customParameters;
}

QList<GameCVar>& GameCreateParams::cvars()
{
	return d->cvars;
}

const QList<GameCVar>& GameCreateParams::cvars() const
{
	return d->cvars;
}

const QString& GameCreateParams::demoPath() const
{
	return d->demoPath;
}

const GameDemo &GameCreateParams::demoRecord() const
{
	return d->demoRecord;
}

void GameCreateParams::setDemoRecord(const GameDemo &demo)
{
	d->demoRecord = demo;
}

QList<DMFlagsSection>& GameCreateParams::dmFlags()
{
	return d->dmFlags;
}

const QList<DMFlagsSection>& GameCreateParams::dmFlags() const
{
	return d->dmFlags;
}

const QString& GameCreateParams::email() const
{
	return d->email;
}

const QString& GameCreateParams::executablePath() const
{
	return d->executablePath;
}

const GameMode& GameCreateParams::gameMode() const
{
	return d->gameMode;
}

GameCreateParams::HostMode GameCreateParams::hostMode() const
{
	return d->hostMode;
}

bool GameCreateParams::isBroadcastToLan() const
{
	return d->broadcastToLan;
}

bool GameCreateParams::isBroadcastToMaster() const
{
	return d->broadcastToMaster;
}

bool GameCreateParams::isRandomMapRotation() const
{
	return d->randomMapRotation;
}

const QString& GameCreateParams::ingamePassword() const
{
	return d->ingamePassword;
}

const QString& GameCreateParams::iwadPath() const
{
	return d->iwadPath;
}

QString GameCreateParams::iwadName() const
{
	QFileInfo fi(iwadPath());
	return fi.fileName();
}

const QString& GameCreateParams::map() const
{
	return d->map;
}

const QStringList& GameCreateParams::mapList() const
{
	return d->mapList;
}

int GameCreateParams::maxClients() const
{
	return d->maxClients;
}

int GameCreateParams::maxPlayers() const
{
	return d->maxPlayers;
}

int GameCreateParams::maxTotalClientSlots() const
{
	return qMax(maxClients(), maxPlayers());
}

const QString& GameCreateParams::motd() const
{
	return d->motd;
}

const QString& GameCreateParams::name() const
{
	return d->name;
}

unsigned short GameCreateParams::port() const
{
	return d->port;
}

QList<bool>& GameCreateParams::pwadsOptional()
{
	return d->pwadsOptional;
}

const QList<bool>& GameCreateParams::pwadsOptional() const
{
	return d->pwadsOptional;
}

QStringList& GameCreateParams::pwadsPaths()
{
	return d->pwadsPaths;
}

const QStringList& GameCreateParams::pwadsPaths() const
{
	return d->pwadsPaths;
}

QStringList GameCreateParams::pwadsNames() const
{
	QStringList result;
	foreach (const QString &path, pwadsPaths())
	{
		QFileInfo fi(path);
		result << fi.fileName();
	}
	return result;
}

QList<PWad> GameCreateParams::pwads() const
{
	QList<PWad> result;
	QStringList names = pwadsNames();
	for (int i = 0; i < names.size(); ++i)
	{
		result << PWad(names[i], pwadsOptional()[i]);
	}
	return result;
}

const QString& GameCreateParams::rconPassword() const
{
	return d->rconPassword;
}

void GameCreateParams::setBroadcastToLan(bool b)
{
	d->broadcastToLan = b;
}

void GameCreateParams::setBroadcastToMaster(bool b)
{
	d->broadcastToMaster = b;
}

void GameCreateParams::setConnectPassword(const QString& pass)
{
	d->connectPassword = pass;
}

void GameCreateParams::setCustomParameters(const QStringList& customParameters)
{
	d->customParameters = customParameters;
}

void GameCreateParams::setCvars(const QList<GameCVar>& cvars)
{
	d->cvars = cvars;
}

void GameCreateParams::setDemoPath(const QString& demoPath)
{
	d->demoPath = demoPath;
}

void GameCreateParams::setEmail(const QString& email)
{
	d->email = email;
}

void GameCreateParams::setExecutablePath(const QString& executablePath)
{
	d->executablePath = executablePath;
}

void GameCreateParams::setGameMode(const GameMode& mode)
{
	d->gameMode = mode;
}

void GameCreateParams::setHostMode(HostMode mode)
{
	d->hostMode = mode;
}

void GameCreateParams::setIngamePassword(const QString& pass)
{
	d->ingamePassword = pass;
}

void GameCreateParams::setIwadPath(const QString& iwadPath)
{
	d->iwadPath = iwadPath;
}

void GameCreateParams::setMap(const QString& map)
{
	d->map = map;
}

void GameCreateParams::setMapList(const QStringList& mapList)
{
	d->mapList = mapList;
}

void GameCreateParams::setMaxClients(int num)
{
	d->maxClients = num;
}

void GameCreateParams::setMaxPlayers(int num)
{
	d->maxPlayers = num;
}

void GameCreateParams::setMotd(const QString& motd)
{
	d->motd = motd;
}

void GameCreateParams::setName(const QString& name)
{
	d->name = name;
}

void GameCreateParams::setPort(unsigned short port)
{
	d->port = port;
}

void GameCreateParams::setPwadsOptional(const QList<bool>& pwadsOptional)
{
	d->pwadsOptional = pwadsOptional;
}

void GameCreateParams::setPwadsPaths(const QStringList& pwadsPaths)
{
	d->pwadsPaths = pwadsPaths;
}

void GameCreateParams::setRandomMapRotation(bool b)
{
	d->randomMapRotation = b;
}

void GameCreateParams::setRconPassword(const QString& pass)
{
	d->rconPassword = pass;
}

void GameCreateParams::setSkill(int skill)
{
	d->skill = skill;
}

void GameCreateParams::setUrl(const QString& url)
{
	d->url = url;
}

int GameCreateParams::skill() const
{
	return d->skill;
}

const QString& GameCreateParams::url() const
{
	return d->url;
}
