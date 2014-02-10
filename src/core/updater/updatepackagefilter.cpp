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
		bool bWasAnyUpdatePackageIgnored;
		QMap<QString, QList<unsigned long long> > ignoredPackagesRevisions;
		QMap<QString, PluginInfo> plugins;
};
//////////////////////////////////////////////////////////////////////////////
UpdatePackageFilter::UpdatePackageFilter()
{
	d = new PrivData();
	d->bWasAnyUpdatePackageIgnored = false;
}

UpdatePackageFilter::~UpdatePackageFilter()
{
	delete d;
}

QMap<QString, UpdatePackageFilter::PluginInfo> UpdatePackageFilter::collectPluginInfo()
{
	QMap<QString, PluginInfo> infos;
	const QList<PluginLoader::Plugin*> plugins = gPlugins->plugins();
	foreach (const PluginLoader::Plugin* plugin, plugins)
	{
		PluginInfo pluginInfo;
		pluginInfo.name = plugin->info()->data()->name;
		pluginInfo.revision = plugin->info()->data()->version;
		QString prefixedName = AutoUpdater::PLUGIN_PREFIX + pluginInfo.name.toLower().replace(" ", "");
		infos.insert(prefixedName, pluginInfo);
	}
	return infos;
}

QList<UpdatePackage> UpdatePackageFilter::filter(const QList<UpdatePackage>& packages)
{
	QList<UpdatePackage> filtered;
	d->plugins = collectPluginInfo();
	QList<UpdatePackage> packagesOnIgnoredList;
	foreach (UpdatePackage pkg, packages)
	{
		if (isDifferentThanInstalled(pkg))
		{
			if (!isOnIgnoredList(pkg.name, pkg.revision))
			{
				filtered << pkg;
			}
			else
			{
				packagesOnIgnoredList << pkg;
			}
		}
	}
	if (!filtered.isEmpty())
	{
		// If we do an update of at least one package, we also need to update
		// all packages that were previously ignored.
		filtered.append(packagesOnIgnoredList);
		packagesOnIgnoredList.clear();
	}
	d->bWasAnyUpdatePackageIgnored = !packagesOnIgnoredList.isEmpty();
	return filtered;
}

bool UpdatePackageFilter::isDifferentThanInstalled(UpdatePackage& pkg) const
{
	if (pkg.name == AutoUpdater::MAIN_PROGRAM_PACKAGE_NAME)
	{
		// Main program node.
		unsigned long long localRevision = Version::revisionNumber();
		if (localRevision != pkg.revision)
		{
			pkg.currentlyInstalledDisplayVersion = Version::versionRevision();
			return true;
		}
	}
	else
	{
		// Plugin node.
		if (d->plugins.contains(pkg.name))
		{
			PluginInfo pluginInfo = d->plugins[pkg.name];
			if (pluginInfo.revision != pkg.revision)
			{
				pkg.currentlyInstalledDisplayVersion = QString::number(pluginInfo.revision);
				return true;
			}
		}
	}
	return false;
}

bool UpdatePackageFilter::isOnIgnoredList(const QString& package,
	unsigned long long revision) const
{
	const QList<unsigned long long>& list = d->ignoredPackagesRevisions[package];
	return list.contains(revision);
}

void UpdatePackageFilter::setIgnoreRevisions(
	const QMap<QString, QList<unsigned long long> >& packagesRevisions)
{
	d->ignoredPackagesRevisions = packagesRevisions;
}

bool UpdatePackageFilter::wasAnyUpdatePackageIgnored() const
{
	return d->bWasAnyUpdatePackageIgnored;
}
