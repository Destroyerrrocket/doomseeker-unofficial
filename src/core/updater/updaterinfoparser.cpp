//------------------------------------------------------------------------------
// updaterinfoparser.cpp
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "updaterinfoparser.h"

#include "updater/autoupdater.h"
#include "updater/updatepackage.h"
#include "json.h"
#include "log.h"

class UpdaterInfoParser::PrivData
{
	public:
		QList<UpdatePackage> packages;
};
//////////////////////////////////////////////////////////////////////////////
UpdaterInfoParser::UpdaterInfoParser()
{
	d = new PrivData();
}

UpdaterInfoParser::~UpdaterInfoParser()
{
	delete d;
}

const QList<UpdatePackage>& UpdaterInfoParser::packages() const
{
	return d->packages;
}

int UpdaterInfoParser::parse(const QByteArray& json)
{
	d->packages.clear();
	QVariant var = QtJson::Json::parse(json);
	if (var.isValid())
	{
		QVariantMap metaData = var.toMap();
		if (metaData.contains(AutoUpdater::MAIN_PROGRAM_PACKAGE_NAME))
		{
			foreach (const QString& package, metaData.keys())
			{
				int result = parsePackageNode(package, metaData[package].toMap());
				if (result != AutoUpdater::EC_Ok)
				{
					return result;
				}
			}
		}
		else
		{
			return AutoUpdater::EC_UpdaterInfoMissingMainProgramNode;
		}
	}
	else
	{
		return AutoUpdater::EC_UpdaterInfoCannotParse;
	}
	return AutoUpdater::EC_Ok;
}

int UpdaterInfoParser::parsePackageNode(const QString& packageName, const QVariantMap& map)
{
	foreach (const QString& channel, map.keys())
	{
		QVariantMap channelInfo = map[channel].toMap();
		UpdatePackage package;
		package.name = packageName;
		package.channel = channel;
		if (channelInfo.contains("revision"))
		{
			package.revision = channelInfo["revision"].toLongLong();
		}
		else
		{
			gLog << tr("Missing update revision info for package %1, channel %2.")
				.arg(packageName, channel);
			return AutoUpdater::EC_MissingRevisionInfo;
		}
		if (channelInfo.contains("display-version"))
		{
			package.displayVersion = channelInfo["display-version"].toString();
		}
		else
		{
			package.displayVersion = QString::number(package.revision);
		}
		if (channelInfo.contains("URL"))
		{
			package.downloadUrl = channelInfo["URL"].toString();
		}
		else
		{
			gLog << tr("Missing update download URL for package %1, channel %2.")
				.arg(packageName, channel);
			return AutoUpdater::EC_MissingDownloadUrl;
		}
		d->packages << package;
	}
	return AutoUpdater::EC_Ok;
}
