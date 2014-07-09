//------------------------------------------------------------------------------
// ip2cupdater.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#include "ip2cupdater.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <zlib.h>

#include "log.h"
#include "version.h"

IP2CUpdater::IP2CUpdater()
{
	pCurrentNetworkReply = NULL;
	pNetworkAccessManager = new FixedNetworkAccessManager();
}

IP2CUpdater::~IP2CUpdater()
{
	if (pCurrentNetworkReply != NULL)
	{
		pCurrentNetworkReply->disconnect();
		pCurrentNetworkReply->abort();
		pCurrentNetworkReply->deleteLater();
	}

	if (pNetworkAccessManager != NULL)
	{
		pNetworkAccessManager->deleteLater();
	}
}

void IP2CUpdater::downloadDatabase(const QUrl& netLocation)
{
	retrievedData.clear();

	QNetworkRequest request;
	request.setUrl(netLocation);
	qDebug() << netLocation;
	request.setRawHeader("User-Agent", Version::userAgent().toAscii());

	pCurrentNetworkReply = pNetworkAccessManager->get(request);
	this->connect(pCurrentNetworkReply, SIGNAL(	downloadProgress(qint64, qint64) ),
				SIGNAL( downloadProgress(qint64, qint64) ) );
	this->connect(pCurrentNetworkReply, SIGNAL(	finished() ),
				SLOT( downloadFinished() ) );
}

void IP2CUpdater::downloadFinished()
{
	QByteArray data = pCurrentNetworkReply->readAll();

	QUrl possibleRedirectUrl = pCurrentNetworkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	QUrl url = pCurrentNetworkReply->request().url();
	if (!possibleRedirectUrl.isEmpty()
		&& possibleRedirectUrl != url)
	{
		// Redirect.
		if (possibleRedirectUrl.isRelative())
		{
			possibleRedirectUrl = url.resolved(possibleRedirectUrl);
		}

		pCurrentNetworkReply->deleteLater();
		downloadDatabase(possibleRedirectUrl);
	}
	else
	{
		pCurrentNetworkReply->deleteLater();
		pCurrentNetworkReply = NULL;

		// First we need to write it to a temporary file
		QTemporaryFile tmpFile;
		if(tmpFile.open())
		{
			tmpFile.write(data);

			QString tmpFilePath = tmpFile.fileName();

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
}

bool IP2CUpdater::getRollbackData()
{
	rollbackData.clear();

	QFile file(pathToFile);
	if (!file.exists())
	{
		return false;
	}

	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	rollbackData = file.readAll();
	file.close();

	return true;
}

bool IP2CUpdater::needsUpdate(const QString& filePath, unsigned minimumUpdateAge)
{
	if (filePath.isEmpty())
	{
		return false;
	}

	if (minimumUpdateAge == 0)
	{
		minimumUpdateAge = 1;
	}

	QFileInfo fileInfo(filePath);
	if (fileInfo.exists())
	{
		QDateTime current = QDateTime::currentDateTime();
		QDateTime lastModified = fileInfo.lastModified();

		int daysTo = lastModified.daysTo(current);

		// Handle file system errors.
		if (daysTo < 0)
		{
			return true;
		}

		return (unsigned)daysTo >= minimumUpdateAge;
	}

	return true;
}

bool IP2CUpdater::rollback()
{
	bool bSuccess = save(rollbackData);
	rollbackData.clear();

	return bSuccess;
}

bool IP2CUpdater::save(const QByteArray& saveWhat)
{
	if (saveWhat.isEmpty())
	{
		return false;
	}

	QFile file(pathToFile);
	if (!file.open(QIODevice::WriteOnly))
	{
		return false;
	}

	file.write(saveWhat);
	file.close();

	return true;
}

bool IP2CUpdater::saveDownloadedData()
{
	return save(retrievedData);
}
