//------------------------------------------------------------------------------
// binaries.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "binaries.h"
#include "configuration/doomseekerconfig.h"
#include "server.h"
#include "main.h"
#include "message.h"
#include "plugins/engineplugin.h"

#include <QString>
#include <QFileInfo>

Binaries::BinaryNamesDictionary Binaries::binaryNames;

Binaries::Binaries(const EnginePlugin *plugin)
{
	enginePlugin = plugin;
	if (binaryNames.empty()) // Is not init yet.
	{
		binaryNames.insert(Client, "client");
		binaryNames.insert(Offline, "offline play");
		binaryNames.insert(TServer, "server");
	}
}

QString Binaries::clientWorkingDirectory(Message& message) const
{
	QFileInfo fi(clientBinary(message));
	return fi.absolutePath();
}

QString	Binaries::obtainBinary(const QString& configKey, BinaryType binaryType, Message& message) const
{
	IniSection config = gConfig.iniSectionForPlugin(plugin());
	IniVariable setting = config[configKey];

	message = Message();
	QString error = "";
	if (setting.value().isNull())
	{
		error = tr("No %1 executable specified for %2").arg(binaryNames[binaryType]).arg(plugin()->data()->name);
		message = Message::customError(error);
		return QString();
	}

	QFileInfo fi(setting);

	if (!fi.exists() || (fi.isDir() && !fi.isBundle()))
	{
		error = tr("Executable for %1 %2:\n%3\nis a directory or doesn't exist.")
					.arg(plugin()->data()->name)
					.arg(binaryNames[binaryType])
					.arg(fi.canonicalFilePath());
		message = Message::customError(error);
		return QString();
	}

	return fi.canonicalFilePath();
}

QString Binaries::offlineGameWorkingDirectory(Message& message) const
{
	// Generated when game is launched. See doxy.
	return QString();
}

QString Binaries::serverWorkingDirectory(Message& message) const
{
	// Generated when game is launched. See doxy.
	return QString();
}
