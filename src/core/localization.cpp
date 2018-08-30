//-----------------------------------------------------------------------------
// localization.cpp
//-----------------------------------------------------------------------------
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//-----------------------------------------------------------------------------
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//-----------------------------------------------------------------------------
#include "localization.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>
#include <QSet>
#include <QStringList>
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include "datapaths.h"
#include "log.h"

const QString DEFINITION_FILE_PATTERN = "*.def";
const QString TRANSLATIONS_LOCATION_SUBDIR = "translations";
const int NUM_VALID_VERSION_TOKENS = 2;
const int NUM_VALID_TOKENS = 3;

QList<QTranslator*> Localization::currentlyLoadedTranslations;

class Localization::LocalizationLoader
{
	public:
		LocalizationLoader();

		QList<LocalizationInfo> loadLocalizationsList(const QStringList& definitionsFileSearchDirs);

	private:
		QList<LocalizationInfo> localizations;

		void loadLocalizationsListFile(const QString& definitionsFilePath);
		void loadLocalizationsListFile(QIODevice& io);

		/**
		 * @brief Reads version information from definition file.
		 *
		 * Version information is always in the first line of the file.
		 * Proper version number is greater than zero.
		 */
		int obtainVersion(QIODevice& io);

		void sort();
};

bool localizationInfoLessThan(const LocalizationInfo &o1, const LocalizationInfo &o2)
{
	return o1.localeName.toLower() < o2.localeName.toLower();
}

QList<LocalizationInfo> Localization::loadLocalizationsList(const QStringList& definitionsFileSearchDirs)
{
	LocalizationLoader l;
	return l.loadLocalizationsList(definitionsFileSearchDirs);
}

bool Localization::loadTranslation(const QString& localeName)
{
	// Out with the old.
	qDeleteAll(currentlyLoadedTranslations);
	currentlyLoadedTranslations.clear();
	if (localeName == "en_EN")
		return true;
	// In with the new.
	QStringList searchPaths = DataPaths::staticDataSearchDirs(
		TRANSLATIONS_LOCATION_SUBDIR);
	// Qt library translator.
	installQtTranslations(localeName, searchPaths);

	// Doomseeker translator.
	bool installed = installTranslation(localeName, searchPaths);

	// Plugins translators.
	foreach (const PluginLoader::Plugin *plugin, gPlugins->plugins())
	{
		QString name = plugin->info()->nameCanonical();
		QTranslator *pluginTranslator = loadTranslationFile(
			QString("%1_%2").arg(name, localeName),
			searchPaths);
		if (pluginTranslator)
		{
			gLog << QString("Loaded translation for plugin %1").arg(name);
			QCoreApplication::installTranslator(pluginTranslator);
			currentlyLoadedTranslations.append(pluginTranslator);
		}
	}
	return installed;
}

void Localization::installQtTranslations(const QString &localeName, QStringList searchPaths)
{
	// First let's try to load translation that is bundled with program.
	// This behavior is valid for Windows.
	//
	// If Qt translation is not bundled with program then try to load
	// it from system location. This behavior is valid for Linux.
	searchPaths << QLibraryInfo::location(QLibraryInfo::TranslationsPath);

#if (QT_VERSION < QT_VERSION_CHECK(5, 3, 0))
	// https://sourceforge.net/p/nootka/hg/ci/d00c13d9223b5bf74802aaec209ebc171efb27ce/tree/src/libs/core/tinitcorelib.cpp?diff=ce9e1fc25b8c23c9713c0cfa4c350d5ac2452fab
	installTranslation("qt_" + localeName, searchPaths);
#else
	installTranslation("qtbase_" + localeName, searchPaths);
	installTranslation("qtmultimedia_" + localeName, searchPaths);
#endif
}

bool Localization::installTranslation(const QString &translationName, const QStringList &searchPaths)
{
	QTranslator* translator = loadTranslationFile(translationName, searchPaths);
	if (translator != NULL)
	{
		QCoreApplication::installTranslator(translator);
		currentlyLoadedTranslations.append(translator);
		return true;
	}
	else
	{
		gLog << QString("Translation '%1' not found.").arg(translationName);
		return false;
	}
}

QTranslator* Localization::loadTranslationFile(const QString& translationName, const QStringList& searchPaths)
{
	QTranslator* pTranslator = new QTranslator();
	bool bLoaded = false;
	foreach (const QString& dir, searchPaths)
	{
		if (pTranslator->load(translationName, dir))
		{
			gLog << QString("Found translation '%1' in dir '%2'.").arg(translationName, dir);
			bLoaded = true;
			break;
		}
	}
	if (!bLoaded)
	{
		delete pTranslator;
		pTranslator = NULL;
	}
	return pTranslator;
}
//////////////////////////////////////////////////////////////////////////////
Localization::LocalizationLoader::LocalizationLoader()
{
	localizations << LocalizationInfo::DEFAULT;
}

QList<LocalizationInfo> Localization::LocalizationLoader::loadLocalizationsList(const QStringList& definitionsFileSearchDirs)
{
	foreach (const QString& dirPath, definitionsFileSearchDirs)
	{
		loadLocalizationsListFile(dirPath);
	}
	sort();
	return localizations;
}

void Localization::LocalizationLoader::loadLocalizationsListFile(const QString& definitionsFilePath)
{
	QDir dir(definitionsFilePath);
	QStringList defFiles = dir.entryList(QStringList(DEFINITION_FILE_PATTERN), QDir::Files);
	foreach (const QString& defFileName, defFiles)
	{
		// No point in translating strings in this class because
		// translation is not loaded yet.
		gLog << QString("Reading localizations definitions file: %1").arg(defFileName);
		QString filePath = dir.absoluteFilePath(defFileName);
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly))
		{
			loadLocalizationsListFile(file);
			file.close();
		}
		else
		{
			gLog << QString("Failed to open localizations definitions file: %1").arg(definitionsFilePath);
		}
	}
}

void Localization::LocalizationLoader::loadLocalizationsListFile(QIODevice& io)
{
	int version = obtainVersion(io);
	if (version <= 0)
	{
		gLog << QString("Translation definition file doesn't contain valid protocol version information.");
		return;
	}

	QString line = io.readLine();
	while (!line.isEmpty())
	{
		line = line.trimmed();
		// Discard empty and comment lines.
		if (!line.isEmpty() && !line.startsWith("#"))
		{
			QStringList tokens = line.split(";");
			if (tokens.size() == NUM_VALID_TOKENS)
			{
				LocalizationInfo info;
				info.countryCodeName = tokens[0].trimmed();
				info.localeName = tokens[1].trimmed();
				info.niceName = tokens[2].trimmed();
				if (!localizations.contains(info))
				{
					localizations.append(info);
				}
			}
			else
			{
				gLog << QString("Invalid localization definition: %1").arg(line);
			}
		}
		line = io.readLine();
	}
}

int Localization::LocalizationLoader::obtainVersion(QIODevice& io)
{
	QString line = io.readLine();
	int version = -1;
	if (!line.isNull())
	{
		// First line contains PROTOCOL_VERSION.
		QStringList tokens = line.split("=");
		if (tokens.size() == NUM_VALID_VERSION_TOKENS)
		{
			QString versionToken = tokens[1];
			bool bOk = false;
			version = versionToken.toInt(&bOk);
			if (!bOk)
			{
				version = -1;
			}
		}
	}
	return version;
}

void Localization::LocalizationLoader::sort()
{
	qSort(localizations.begin(), localizations.end(), localizationInfoLessThan);
}
