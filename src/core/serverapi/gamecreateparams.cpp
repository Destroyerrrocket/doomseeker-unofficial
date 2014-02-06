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

class GameCreateParams::PrivData
{
	public:
		QStringList customParameters;
		QList<GameCVar> cvars;
		QString executablePath;
		QString demoPath;
		QList<DMFlagsSection> dmFlags;
		QString iwadPath;
		QStringList pwadsPaths;
};


COPYABLE_D_POINTERED_DEFINE(GameCreateParams);


GameCreateParams::GameCreateParams()
{
	d = new PrivData();
}

GameCreateParams::~GameCreateParams()
{
	delete d;
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

QList<DMFlagsSection> GameCreateParams::dmFlags()
{
	return d->dmFlags;
}

const QList<DMFlagsSection>& GameCreateParams::dmFlags() const
{
	return d->dmFlags;
}

const QString& GameCreateParams::executablePath() const
{
	return d->executablePath;
}

const QString& GameCreateParams::iwadPath() const
{
	return d->iwadPath;
}

QStringList& GameCreateParams::pwadsPaths()
{
	return d->pwadsPaths;
}

const QStringList& GameCreateParams::pwadsPaths() const
{
	return d->pwadsPaths;
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

void GameCreateParams::setExecutablePath(const QString& executablePath)
{
	d->executablePath = executablePath;
}

void GameCreateParams::setIwadPath(const QString& iwadPath)
{
	d->iwadPath = iwadPath;
}

void GameCreateParams::setPwadsPaths(const QStringList& pwadsPaths)
{
	d->pwadsPaths = pwadsPaths;
}
