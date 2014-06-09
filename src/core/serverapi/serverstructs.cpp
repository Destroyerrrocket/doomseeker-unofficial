//------------------------------------------------------------------------------
// serverstructs.cpp
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
#include "serverstructs.h"

#include <QVector>

class DMFlag::PrivData
{
	public:
		QString name;
		unsigned value;
};

DMFlag::DMFlag()
{
	d = new PrivData();
	d->value = 0;
}

DMFlag::DMFlag(QString name, unsigned value)
{
	d = new PrivData();
	d->name = name;
	d->value = value;
}

COPYABLE_D_POINTERED_DEFINE(DMFlag);

DMFlag::~DMFlag()
{
	delete d;
}

bool DMFlag::isValid() const
{
	return value() > 0;
}

const QString& DMFlag::name() const
{
	return d->name;
}

unsigned DMFlag::value() const
{
	return d->value;
}

////////////////////////////////////////////////////////////////////////////////

class DMFlagsSection::PrivData
{
	public:
		QString name;
		QVector<DMFlag> flags;
};

COPYABLE_D_POINTERED_DEFINE(DMFlagsSection);

DMFlagsSection::DMFlagsSection()
{
	d = new PrivData();
}

DMFlagsSection::DMFlagsSection(const QString& name)
{
	d = new PrivData();
	d->name = name;
}

DMFlagsSection::~DMFlagsSection()
{
	delete d;
}

void DMFlagsSection::add(const DMFlag& flag)
{
	d->flags << flag;
}

unsigned DMFlagsSection::combineValues() const
{
	unsigned result = 0;
	foreach (const DMFlag& flag, d->flags)
	{
		result |= flag.value();
	}
	return result;
}

int DMFlagsSection::count() const
{
	return d->flags.count();
}

bool DMFlagsSection::isEmpty() const
{
	return count() == 0;
}

const QString& DMFlagsSection::name() const
{
	return d->name;
}

const DMFlag& DMFlagsSection::operator[](int index) const
{
	return d->flags[index];
}

DMFlag& DMFlagsSection::operator[](int index)
{
	return d->flags[index];
}

////////////////////////////////////////////////////////////////////////////////

class GameCVar::PrivData
{
	public:
		QString command;
		QString name;
		QVariant value;
};

COPYABLE_D_POINTERED_DEFINE(GameCVar);

GameCVar::GameCVar()
{
	d = new PrivData();
}

GameCVar::GameCVar(const QString &name, const QString &command)
{
	d = new PrivData();
	d->name = name;
	d->command = command;
}

GameCVar::~GameCVar()
{
	delete d;
}

const QString &GameCVar::command() const
{
	return d->command;
}

bool GameCVar::hasValue() const
{
	return value().isValid();
}

bool GameCVar::isValid() const
{
	return !command().isEmpty();
}

const QString &GameCVar::name() const
{
	return d->name;
}

void GameCVar::setValue(const QVariant& value)
{
	d->value = value;
}

const QVariant &GameCVar::value() const
{
	return d->value;
}

////////////////////////////////////////////////////////////////////////////////

class GameMode::PrivData
{
	public:
		int index;
		QString name;
		bool teamgame;
};

COPYABLE_D_POINTERED_DEFINE(GameMode);

GameMode::GameMode()
{
	d = new PrivData();
	d->index = -1;
	d->teamgame = false;
}

GameMode::GameMode(int index, const QString &name)
{
	d = new PrivData();
	d->index = index;
	d->name = name;
	d->teamgame = false;
}

GameMode::~GameMode()
{
	delete d;
}

GameMode GameMode::ffaGame(int index, const QString &name)
{
	GameMode result(index, name);
	result.setTeamGame(false);
	return result;
}

int GameMode::index() const
{
	return d->index;
}

GameMode GameMode::mkCooperative()
{
	return ffaGame(SGM_Cooperative, QObject::tr("Cooperative"));
}

GameMode GameMode::mkDeathmatch()
{
	return ffaGame(SGM_Deathmatch, QObject::tr("Deathmatch"));
}

GameMode GameMode::mkTeamDeathmatch()
{
	return teamGame(SGM_TeamDeathmatch, QObject::tr("Team DM"));
}

GameMode GameMode::mkCaptureTheFlag()
{
	return teamGame(SGM_CTF, QObject::tr("CTF"));
}

GameMode GameMode::mkUnknown()
{
	return ffaGame(SGM_Unknown, QObject::tr("Unknown"));
}

const QString &GameMode::name() const
{
	return d->name;
}

bool GameMode::isTeamGame() const
{
	return d->teamgame;
}

bool GameMode::isValid() const
{
	return !d->name.isEmpty();
}

void GameMode::setTeamGame(bool b)
{
	d->teamgame = b;
}

GameMode GameMode::teamGame(int index, const QString &name)
{
	GameMode result(index, name);
	result.setTeamGame(true);
	return result;
}

////////////////////////////////////////////////////////////////////////////////

class PWad::PrivData
{
	public:
		QString name;
		bool optional;
};

COPYABLE_D_POINTERED_DEFINE(PWad);

PWad::PWad(const QString &name, bool optional)
{
	d = new PrivData();
	d->name = name;
	d->optional = optional;
}

PWad::~PWad()
{
	delete d;
}

bool PWad::isOptional() const
{
	return d->optional;
}

const QString& PWad::name() const
{
	return d->name;
}

