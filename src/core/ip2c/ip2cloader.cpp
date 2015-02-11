#include "ip2cloader.h"

#include "configuration/doomseekerconfig.h"
#include "doomseekerfilepaths.h"
#include "ip2c/ip2c.h"
#include "ip2c/ip2cparser.h"
#include "ip2c/ip2cupdater.h"
#include "log.h"
#include <QFile>

DClass<IP2CLoader>
{
	public:
		IP2CParser* ip2cParser;
		IP2CUpdater* ip2cUpdater;
};

DPointered(IP2CLoader)

///////////////////////////////////////////////////////////////////////////////
IP2CLoader::IP2CLoader()
{
	d->ip2cParser = new IP2CParser(IP2C::instance());
	this->connect(d->ip2cParser, SIGNAL( parsingFinished(bool) ),
		SLOT( ip2cFinishedParsing(bool) ) );

	d->ip2cUpdater = new IP2CUpdater();
	d->ip2cUpdater->setFilePath(DoomseekerFilePaths::ip2cDatabase());
	this->connect(d->ip2cUpdater, SIGNAL( databaseDownloadFinished(const QByteArray&) ),
		SLOT( ip2cFinishUpdate(const QByteArray&) ) );
	this->connect(d->ip2cUpdater, SIGNAL( downloadProgress(qint64, qint64) ),
		SIGNAL( downloadProgress(qint64, qint64) ) );
}

IP2CLoader::~IP2CLoader()
{
	if (d->ip2cParser->isParsing())
	{
		gLog << tr("IP2C parser is still working. Program will close once this job is done.");
		while (d->ip2cParser->isParsing());
	}

	delete d->ip2cParser;
	delete d->ip2cUpdater;
}

void IP2CLoader::load()
{
	bool bParseIP2CDatabase = true;
	bool bPerformAutomaticIP2CUpdates = gConfig.doomseeker.bIP2CountryAutoUpdate;

	if (bPerformAutomaticIP2CUpdates)
	{
		int maxAge = gConfig.doomseeker.ip2CountryDatabaseMaximumAge;

		QString databasePath = DoomseekerFilePaths::ip2cDatabase();
		if (IP2CUpdater::needsUpdate(databasePath, maxAge))
		{
			ip2cStartUpdate();
			bParseIP2CDatabase = false;
		}
	}

	if (bParseIP2CDatabase)
	{
		ip2cParseDatabase();
	}
}

void IP2CLoader::update()
{
	ip2cStartUpdate();
}

void IP2CLoader::ip2cFinishUpdate(const QByteArray& downloadedData)
{
	if (!downloadedData.isEmpty())
	{
		gLog << tr("IP2C database finished downloading.");
		QString filePath = DoomseekerFilePaths::ip2cDatabase();
		d->ip2cUpdater->getRollbackData();
		if (!d->ip2cUpdater->saveDownloadedData())
		{
			gLog << tr("Unable to save IP2C database at path: %1").arg(filePath);
		}
		ip2cParseDatabase();
	}
	else
	{
		gLog << tr("IP2C download has failed.");
		ip2cJobsFinished();
	}
}

void IP2CLoader::ip2cFinishedParsing(bool bSuccess)
{
	QString filePath = DoomseekerFilePaths::ip2cDatabase();

	if (!bSuccess)
	{
		gLog << tr("Failed to read IP2C database. Reverting...");

		if (d->ip2cUpdater == NULL || !d->ip2cUpdater->hasRollbackData())
		{
			gLog << tr("IP2C revert attempt failed. Nothing to go back to.");

			// Delete file in this case.
			QFile file(filePath);
			file.remove();

			gLog << tr("Using precompiled IP2C database.");
			d->ip2cParser->readDatabaseThreaded(DoomseekerFilePaths::IP2C_QT_SEARCH_PATH);
		}
		else
		{
			// Revert to old content.
			d->ip2cUpdater->rollback();

			// Must succeed now.
			d->ip2cParser->readDatabaseThreaded(filePath);
		}
	}
	else
	{
		if (d->ip2cUpdater != NULL)
		{
			gLog << tr("IP2C database updated successfully.");
		}

		ip2cJobsFinished();
	}
}

void IP2CLoader::ip2cJobsFinished()
{
	IP2C::instance()->setDataAccessLockEnabled(false);

	if (d->ip2cParser != NULL)
	{
		gLog << tr("IP2C parsing finished.");
	}

	if (d->ip2cUpdater != NULL)
	{
		gLog << tr("IP2C update finished.");
	}
	emit finished();
}

void IP2CLoader::ip2cParseDatabase()
{
	QString filePath = DoomseekerFilePaths::IP2C_QT_SEARCH_PATH;

	gLog << tr("Please wait. IP2C database is being read and converted if necessary. This may take some time.");
	// Attempt to read IP2C database.

	IP2C::instance()->setDataAccessLockEnabled(true);
	d->ip2cParser->readDatabaseThreaded(filePath);
}

void IP2CLoader::ip2cStartUpdate()
{
	gLog << tr("Starting IP2C update.");
	IP2C::instance()->setDataAccessLockEnabled(true);
	QString downloadUrl = gConfig.doomseeker.ip2CountryUrl;
	d->ip2cUpdater->downloadDatabase(downloadUrl);
}
