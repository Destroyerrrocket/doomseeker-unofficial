//------------------------------------------------------------------------------
// chocolatedoomgameexefactory.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "chocolatedoomgameexefactory.h"

#include <serverapi/gamefile.h>

static QStringList searchSuffixes()
{
	QStringList templates;
	templates << "chocolate-%1" << "chocolate %1" << "chocolate%1" << "chocolate_%1";

	QStringList games;
	games << "doom" << "heretic" << "hexen" << "strife";

	QStringList suffixes;
	foreach (const QString &tmplate, templates)
	{
		foreach (const QString &game, games)
		{
			suffixes << tmplate.arg(game);
		}
	}
	return suffixes;
}

DClass<ChocolateDoomGameExeFactory>
{
public:
};
DPointered(ChocolateDoomGameExeFactory)

ChocolateDoomGameExeFactory::ChocolateDoomGameExeFactory(EnginePlugin *plugin)
: GameExeFactory(plugin)
{
	set_gameFiles(&ChocolateDoomGameExeFactory::gameFiles_);
}

ChocolateDoomGameExeFactory::~ChocolateDoomGameExeFactory()
{
}

GameFile ChocolateDoomGameExeFactory::executableForIwad(const QString &iwad)
{
	GameFileList list = gameFiles();
	if (iwad == "hexen.wad")
	{
		return list.findByConfigName("ChocolateHexenBinaryPath");
	}
	else if (iwad == "heretic.wad" || iwad == "heretic1.wad")
	{
		return list.findByConfigName("ChocolateHereticBinaryPath");
	}
	else if (iwad == "strife.wad" || iwad == "strife0.wad")
	{
		return list.findByConfigName("ChocolateStrifeBinaryPath");
	}
	return list.findByConfigName("BinaryPath");
}

GameFileList ChocolateDoomGameExeFactory::gameFiles_() const
{
	return gameFiles();
}

GameFileList ChocolateDoomGameExeFactory::gameFiles()
{
	GameFileList list;
	GameFile tmplate = GameFile().setSearchSuffixes(searchSuffixes()).setCsoModesExecutable(true);

	list << GameFile(tmplate).setConfigName("BinaryPath") // Default
		.setNiceName(tr("Chocolate Doom")).setFileName("chocolate-doom");

	list << GameFile(tmplate).setConfigName("ChocolateHexenBinaryPath")
		.setNiceName(tr("Chocolate Hexen")).setFileName("chocolate-hexen");

	list << GameFile(tmplate).setConfigName("ChocolateHereticBinaryPath")
		.setNiceName(tr("Chocolate Heretic")).setFileName("chocolate-heretic");

	list << GameFile(tmplate).setConfigName("ChocolateStrifeBinaryPath")
		.setNiceName(tr("Chocolate Strife")).setFileName("chocolate-strife");

	return list;
}
