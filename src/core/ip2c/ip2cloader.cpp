#include "ip2cloader.h"

#include "configuration/doomseekerconfig.h"
#include "doomseekerfilepaths.h"
#include "ip2c/ip2c.h"
#include "ip2c/ip2cparser.h"
#include "ip2c/ip2cupdater.h"
#include "global.h"
#include "log.h"
#include <QFile>
#include <QTimer>

DClass<IP2CLoader>
{
	public:
		IP2CParser* ip2cParser;
		IP2CUpdater* ip2cUpdater;
		bool updateInProgress;
};

DPointered(IP2CLoader)

///////////////////////////////////////////////////////////////////////////////
IP2CLoader::IP2CLoader(QObject *parent)
:QObject(parent)
{
	d->updateInProgress = false;

	d->ip2cParser = new IP2CParser(IP2C::instance());
	this->connect(d->ip2cParser, SIGNAL( parsingFinished(bool) ),
		SLOT( ip2cFinishedParsing(bool) ) );

	d->ip2cUpdater = new IP2CUpdater();
	d->ip2cUpdater->setFilePath(DoomseekerFilePaths::ip2cDatabase());
	this->connect(d->ip2cUpdater, SIGNAL( databaseDownloadFinished(const QByteArray&) ),
		SLOT( ip2cFinishUpdate(const QByteArray&) ) );
	this->connect(d->ip2cUpdater, SIGNAL( downloadProgress(qint64, qint64) ),
		SIGNAL( downloadProgress(qint64, qint64) ) );
	this->connect(d->ip2cUpdater, SIGNAL(updateNeeded(int)),
		SLOT(onUpdateNeeded(int)));
}

IP2CLoader::~IP2CLoader()
{
	if (d->ip2cParser->isParsing())
	{
		gLog << tr("IP2C parser is still working, awaiting stop...");
		while (d->ip2cParser->isParsing())
		{
			Sleep::sleep(1);
		}
	}

	delete d->ip2cParser;
	delete d->ip2cUpdater;
}

void IP2CLoader::load()
{
	if (gConfig.doomseeker.bIP2CountryAutoUpdate)
	{
		QString databasePath = DoomseekerFilePaths::ip2cDatabase();
		d->ip2cUpdater->needsUpdate(databasePath);
	}
	ip2cParseDatabase();
}

void IP2CLoader::onUpdateNeeded(int status)
{
	if (status == IP2CUpdater::UpdateNeeded)
	{
		update();
	}
	else
	{
		switch (status)
		{
		case IP2CUpdater::UpToDate:
			gLog << tr("IP2C update not needed.");
			break;
		case IP2CUpdater::UpdateCheckError:
			gLog << tr("IP2C update errored. See log for details.");
			break;
		default:
			gLog << tr("IP2C update bugged out.");
			break;
		}
		ip2cJobsFinished();
	}
}

void IP2CLoader::update()
{
	d->updateInProgress = true;
	if (!d->ip2cParser->isParsing())
	{
		gLog << tr("Starting IP2C update.");
		IP2C::instance()->setDataAccessLockEnabled(true);
		d->ip2cUpdater->downloadDatabase();
	}
	else
	{
		// Delay in hope that parser finishes
		gLog << tr("IP2C update must wait until parsing of current database finishes. "
			"Waiting 1 second");
		QTimer::singleShot(1000, this, SLOT(update()));
	}
}

void IP2CLoader::ip2cFinishUpdate(const QByteArray& downloadedData)
{
	d->updateInProgress = false;
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
		gLog << tr("IP2C parsing finished.");
		ip2cJobsFinished();
	}
}

void IP2CLoader::ip2cJobsFinished()
{
	if (!d->ip2cUpdater->isWorking() && !d->ip2cParser->isParsing() && !d->updateInProgress)
	{
		IP2C::instance()->setDataAccessLockEnabled(false);
		emit finished();
	}
}

void IP2CLoader::ip2cParseDatabase()
{
	QString filePath = DoomseekerFilePaths::IP2C_QT_SEARCH_PATH;

	gLog << tr("Please wait. IP2C database is being read. This may take some time.");
	// Attempt to read IP2C database.

	IP2C::instance()->setDataAccessLockEnabled(true);
	d->ip2cParser->readDatabaseThreaded(filePath);
}
