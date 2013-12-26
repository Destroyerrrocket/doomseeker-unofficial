//------------------------------------------------------------------------------
// gameexeretriever.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "gameexeretriever.h"

#include "serverapi/exefile.h"
#include "serverapi/gameexefactory.h"

GameExeRetriever::GameExeRetriever(GameExeFactory& factory)
: factory(factory)
{
}

QString GameExeRetriever::pathToOfflineExe(Message& message)
{
	ExeFile* f = factory.offline();
	QString path = f->pathToExe(message);
	delete f;
	return path;
}

QString GameExeRetriever::pathToServerExe(Message& message)
{
	ExeFile* f = factory.server();
	QString path = f->pathToExe(message);
	delete f;
	return path;
}

QString GameExeRetriever::offlineWorkingDir(Message& message)
{
	ExeFile* f = factory.offline();
	QString path = f->workingDirectory(message);
	delete f;
	return path;
}

QString GameExeRetriever::serverWorkingDir(Message& message)
{
	ExeFile* f = factory.server();
	QString path = f->workingDirectory(message);
	delete f;
	return path;
}
