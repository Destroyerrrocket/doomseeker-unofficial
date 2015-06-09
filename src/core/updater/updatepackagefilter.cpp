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
#include "strings.h"
#include "version.h"
#include <QCoreApplication>
#include <QFile>
#include <cassert>
#include <wadseeker/wadseekerversioninfo.h>

class UpdatePackageFilter::PluginInfo
{
	public:
		QString name;
		QString revision;
};
//////////////////////////////////////////////////////////////////////////////
DClass<UpdatePackageFilter>
{
	public:
		bool bWasAnyUpdatePackageIgnored;
		bool useFallbackMainProgramName;
		QMap<QString, QList<QString> > ignoredPackagesRevisions;
		QMap<QString, UpdatePackageFilter::PluginInfo> plugins;

		bool hasMainProgramPackage(const QList<UpdatePackage> &packages) const
		{
			foreach (const UpdatePackage &pkg, packages)
			{
				if (pkg.name == AutoUpdater::MAIN_PROGRAM_PACKAGE_NAME)
				{
					return true;
				}
			}
			return false;
		}

		const QString &mainProgramPackageNameOrFallback() const
		{
			return !useFallbackMainProgramName ?
				AutoUpdater::MAIN_PROGRAM_PACKAGE_NAME :
				AutoUpdater::FALLBACK_MAIN_PROGRAM_PACKAGE_NAME;
		}
};

DPointered(UpdatePackageFilter)
//////////////////////////////////////////////////////////////////////////////
UpdatePackageFilter::UpdatePackageFilter()

{
	d->bWasAnyUpdatePackageIgnored = false;
}

UpdatePackageFilter::~UpdatePackageFilter()
{
}

QMap<QString, UpdatePackageFilter::PluginInfo> UpdatePackageFilter::collectPluginInfo()
{
	QMap<QString, PluginInfo> infos;
	const QList<PluginLoader::Plugin*> plugins = gPlugins->plugins();
	foreach (const PluginLoader::Plugin* plugin, plugins)
	{
		PluginInfo pluginInfo;
		pluginInfo.name = plugin->info()->data()->name;
		pluginInfo.revision = QString::number(plugin->info()->data()->version);
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
	d->useFallbackMainProgramName = !d->hasMainProgramPackage(packages);
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
	if (pkg.name == d->mainProgramPackageNameOrFallback())
	{
		QString localRevision = QString::number(Version::revisionNumber());
		if (localRevision != pkg.revision)
		{
			pkg.currentlyInstalledDisplayVersion = Version::versionRevision();
			return true;
		}
	}
	else if (pkg.name == AutoUpdater::WADSEEKER_PACKAGE_NAME)
	{
		QString localRevision = WadseekerVersionInfo::version();
		if (localRevision != pkg.revision)
		{
			pkg.currentlyInstalledDisplayVersion = localRevision;
			return true;
		}
	}
	else if (pkg.name == AutoUpdater::QT_PACKAGE_NAME)
	{
		if (QString(Version::qtPackageVersion()) != pkg.revision)
		{
			pkg.currentlyInstalledDisplayVersion = Version::qtPackageVersion();
			return true;
		}
		// Workaround for auto-updater bug that made itself apparent
		// in Doomseeker 1.1~beta builds. Bug caused every even
		// numbered file in auxiliary packages not to be installed. As
		// Qt is such auxiliary package in 1.1~beta, some files,
		// including Qt5WinExtras.dll, failed to install.
		bool checkWinExtras = false;
#ifdef Q_OS_WIN32
		checkWinExtras = true;
#endif
		if (checkWinExtras)
		{
			QString winExtrasLocation = Strings::combinePaths(
				QCoreApplication::applicationDirPath(), "Qt5WinExtras.dll");
			QFile file(winExtrasLocation);
			if (!file.exists())
			{
				pkg.currentlyInstalledDisplayVersion = Version::qtPackageVersion() + tr("-BROKEN");
				return true;
			}
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
				pkg.currentlyInstalledDisplayVersion = pluginInfo.revision;
				return true;
			}
		}
	}
	return false;
}

bool UpdatePackageFilter::isOnIgnoredList(const QString& package, const QString &revision) const
{
	const QList<QString>& list = d->ignoredPackagesRevisions[package];
	return list.contains(revision);
}

void UpdatePackageFilter::setIgnoreRevisions(
	const QMap<QString, QList<QString> >& packagesRevisions)
{
	d->ignoredPackagesRevisions = packagesRevisions;
}

bool UpdatePackageFilter::wasAnyUpdatePackageIgnored() const
{
	return d->bWasAnyUpdatePackageIgnored;
}
