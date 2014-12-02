//------------------------------------------------------------------------------
// gamedemo.cpp
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
#include "gamedemo.h"

#include "ini/ini.h"
#include "ini/settingsproviderqt.h"
#include "plugins/engineplugin.h"
#include "datapaths.h"
#include <QDateTime>
#include <QDir>

GameDemo::GameDemo()
{
	d.control = NoDemo;
}

GameDemo::GameDemo(Control control)
{
	d.control = control;
}

QString GameDemo::mkDemoFullPath(Control control, const EnginePlugin &plugin)
{
	if (control == Managed)
	{
		return gDefaultDataPaths->demosDirectoryPath() + QDir::separator() + mkDemoName(plugin);
	}
	else
	{
		return mkDemoName(plugin);
	}
}

QString GameDemo::mkDemoName(const EnginePlugin &plugin)
{
	QString demoName = "";
	demoName += QString("%1_%2").
		arg(plugin.data()->name).
		arg(QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss"));
	if (!plugin.data()->demoExtensionAutomatic)
	{
		demoName += QString(".%1").arg(plugin.data()->demoExtension);
	}
	return demoName;
}

void GameDemo::saveDemoMetaData(const QString &demoName, const EnginePlugin &plugin,
	const QString &iwad, const QStringList &pwads)
{
	QString metaFileName;
	// If the extension is automatic we need to add it here
	if (plugin.data()->demoExtensionAutomatic)
	{
		metaFileName = QString("%1.%2.ini").arg(demoName)
			.arg(plugin.data()->demoExtension);
	}
	else
	{
		metaFileName = demoName + ".ini";
	}

	QSettings settings(metaFileName, QSettings::IniFormat);
	SettingsProviderQt settingsProvider(&settings);
	Ini metaFile(&settingsProvider);
	IniSection metaSection = metaFile.createSection("meta");

	metaSection.createSetting("iwad", iwad.toLower());
	metaSection.createSetting("pwads", pwads.join(";"));
}

GameDemo::operator GameDemo::Control() const
{
	return d.control;
}
