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
		LocalizationLoader() {};
		
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
	// In with the new.
	QStringList searchPaths = DataPaths::staticDataSearchDirs(
		TRANSLATIONS_LOCATION_SUBDIR);
	// Qt library translator.
	// First let's try to load translation that is bundled with program.
	// This behavior is valid for Windows.
	QTranslator* qtTranslator = loadTranslationFile("qt_" + localeName, searchPaths);
	if (qtTranslator == NULL)
	{
		// If Qt translation is not bundled with program then try to load
		// it from system location. This behavior is valid for Linux.
		qtTranslator = new QTranslator();
		qtTranslator->load("qt_" + localeName,
			QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		QCoreApplication::installTranslator(qtTranslator);
	}
	currentlyLoadedTranslations.append(qtTranslator);

	// Doomseeker translator.
	QTranslator* myappTranslator = loadTranslationFile(localeName, searchPaths);
	if (myappTranslator != NULL)
	{
		currentlyLoadedTranslations.append(myappTranslator);
	}
	return myappTranslator != NULL;
}

QTranslator* Localization::loadTranslationFile(const QString& localeName, const QStringList& searchPaths)
{
	QTranslator* pTranslator = new QTranslator();
	bool bLoaded = false;
	foreach (const QString& dir, searchPaths)
	{
		if (pTranslator->load(localeName, dir))
		{
			QCoreApplication::installTranslator(pTranslator);
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
		// First line contains protocol version.
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
