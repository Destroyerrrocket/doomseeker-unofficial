//------------------------------------------------------------------------------
// ip2cupdater.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ip2cupdater.h"

#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <zlib.h>
#include "version.h"

IP2CUpdater::IP2CUpdater()
{
	www = new WWW();
	www->setUserAgent(QString("Doomseeker/") + QString(VERSION));
	
	connect(www, SIGNAL( fileDone(QByteArray&, const QString&) ), this, SLOT( processHttp(QByteArray&, const QString&) ));
	connect(www, SIGNAL( downloadProgress(int, int) ), this, SLOT( downloadProgress(int, int) ));	
}

IP2CUpdater::~IP2CUpdater()
{
	delete www;
}

void IP2CUpdater::downloadDatabase(const QUrl& netLocation)
{
	retrievedData.clear();
	www->getUrl(netLocation);
}

void IP2CUpdater::downloadProgressSlot(int value, int max)
{
	emit downloadProgress(value, max);
}

bool IP2CUpdater::needsUpdate(const QString& filePath)
{
	if (filePath.isEmpty())
	{
		return false;
	}

	QFileInfo fileInfo(filePath);
	if (fileInfo.exists())
	{
		// Currently there are no other criteria, if file exists
		// it doesn't need to be downloaded.
		return false;
	}

	return true;
}

void IP2CUpdater::processHttp(QByteArray& data, const QString& filename)
{
	// First we need to write it to a temporary file
	QTemporaryFile tmpFile;
	if(tmpFile.open())
	{
		tmpFile.write(data);
		
		QString tmpFilePath = QDir::tempPath() + QDir::separator() + tmpFile.fileName();

		QByteArray uncompressedData;
		gzFile gz = gzopen(tmpFilePath.toAscii().constData(), "rb");
		if(gz != NULL)
		{
			char chunk[131072]; // 128k
			int bytesRead = 0;
			while((bytesRead = gzread(gz, chunk, 131072)) != 0)
			{
				uncompressedData.append(QByteArray(chunk, bytesRead));
			}
			gzclose(gz);

			retrievedData = uncompressedData;
		}
	}
	
	emit databaseDownloadFinished(retrievedData);
}
