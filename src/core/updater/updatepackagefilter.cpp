//------------------------------------------------------------------------------
// updatepackagefilter.cpp
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
#include "updatepackagefilter.h"

#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include "updater/autoupdater.h"
#include "main.h"
#include "version.h"
#include <cassert>

class UpdatePackageFilter::PluginInfo
{
	public:
		QString name;
		unsigned long long revision;
};
//////////////////////////////////////////////////////////////////////////////
class UpdatePackageFilter::PrivData
{
	public:
		UpdateChannel channel;
};
//////////////////////////////////////////////////////////////////////////////
UpdatePackageFilter::UpdatePackageFilter()
{
	d = new PrivData();
}

UpdatePackageFilter::~UpdatePackageFilter()
{
	delete d;
}

QMap<QString, UpdatePackageFilter::PluginInfo> UpdatePackageFilter::collectPluginInfo()
{
	QMap<QString, PluginInfo> infos;
	const QList<PluginLoader::Plugin* > plugins = Main::enginePlugins->plugins();
	foreach (const PluginLoader::Plugin* plugin, plugins)
	{
		PluginInfo pluginInfo;
		pluginInfo.name = plugin->info->data()->name;
		pluginInfo.revision = plugin->info->data()->version;
		QString prefixedName = AutoUpdater::PLUGIN_PREFIX + pluginInfo.name.toLower();
		infos.insert(prefixedName, pluginInfo);
	}
	return infos;
}

QList<UpdatePackage> UpdatePackageFilter::filter(const QList<UpdatePackage>& packages)
{
	assert(!d->channel.isNull() && "No update channel specified");
	QList<UpdatePackage> filtered;
	QMap<QString, PluginInfo> plugins = collectPluginInfo();
	foreach (const UpdatePackage& pkg, packages)
	{
		if (pkg.channel == d->channel.name())
		{
			if (pkg.name == AutoUpdater::MAIN_PROGRAM_PACKAGE_NAME)
			{
				// Main program node.
				unsigned long long localRevision = Version::revisionNumber();
				if (localRevision != pkg.revision)
				{
					filtered << pkg;
				}
			}
			else
			{
				// Plugin node.
				if (plugins.contains(pkg.name))
				{
					PluginInfo pluginInfo = plugins[pkg.name];
					if (pluginInfo.revision != pkg.revision)
					{
						filtered << pkg;
					}
				}
			}
			filtered << pkg;
		}
	}
	return filtered;
}

void UpdatePackageFilter::setChannel(const UpdateChannel& channel)
{
	d->channel = channel;
}
