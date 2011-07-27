//------------------------------------------------------------------------------
// wadseeker.cpp
//------------------------------------------------------------------------------
//
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
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "protocols/idgames.h"
#include "entities/waddownloadinfo.h"
#include "wwwseeker/entities/fileseekinfo.h"
#include "wwwseeker/wwwseeker.h"
#include "zip/unzip.h"
#include "zip/un7zip.h"
#include "speedcalculator.h"
#include "wadseeker.h"
#include "wadseekerversioninfo.h"
#include <QDir>
#include <QFileInfo>
#include <QStack>

const QString Wadseeker::defaultSites[] =
{
	QString("http://doom.dogsoft.net/getwad.php?search=%WADNAME%"),
	QString("http://www.wadhost.fathax.com/files/"),
//	QString("http://hs.keystone.gr/lap/"),
	QString("http://www.rarefiles.com/download/"),
	QString("http://wads.coffeenet.org/zips/"),
//	QString("http://wads.interdoom.com/"),
	QString("http://static.totaltrash.org/wads/"),
	QString("") // empty url is treated here like an '\0' in a string
};

const QString Wadseeker::forbiddenWads[] =
{
	"attack", "blacktwr", "bloodsea", "canyon", "catwalk", "combine", "doom",
	"doom2", "fistula", "garrison", "geryon", "heretic", "hexen", "hexdd",
	"manor", "mephisto", "minos", "nessus", "paradox", "plutonia", "subspace",
	"subterra", "teeth", "tnt", "ttrap", "strife1", "vesperas", "virgil",
	"voices", ""
};
///////////////////////////////////////////////////////////////////////
Wadseeker::Wadseeker()
{
	d.seekParametersForCurrentSeek = NULL;
	d.wwwSeeker = new WWWSeeker();

	// Forward signals up to outside of the library.
	this->connect(d.wwwSeeker, SIGNAL( finished() ),
		SLOT( wwwSeekerFinished() ) );
	this->connect(d.wwwSeeker, SIGNAL( siteFinished(const QUrl&) ),
		SIGNAL( siteFinished(const QUrl&) ) );
	this->connect(d.wwwSeeker, SIGNAL( siteProgress(const QUrl&, qint64, qint64) ),
		SIGNAL( siteProgress(const QUrl&, qint64, qint64) ) );
	this->connect(d.wwwSeeker, SIGNAL( siteRedirect(const QUrl&, const QUrl&) ),
		SIGNAL( siteRedirect(const QUrl&, const QUrl&) ) );
	this->connect(d.wwwSeeker, SIGNAL( siteStarted(const QUrl&) ),
		SIGNAL( siteStarted(const QUrl&) ) );
}

Wadseeker::~Wadseeker()
{
	delete d.wwwSeeker;
}

void Wadseeker::abort()
{
	d.wwwSeeker->abort();
}

void Wadseeker::cleanUpAfterFinish()
{
	delete d.seekParametersForCurrentSeek;
	d.seekParametersForCurrentSeek = NULL;

	emit allDone(false);
}

const QString Wadseeker::defaultIdgamesUrl()
{
	return Idgames::defaultIdgamesUrl();
}

QStringList Wadseeker::defaultSitesListEncoded()
{
	QStringList list;
	for (int i = 0; !defaultSites[i].isEmpty(); ++i)
	{
		list << QUrl::toPercentEncoding(defaultSites[i]);
	}
	return list;
}

//void Wadseeker::fileDone(QByteArray& data, const QString& filename)
//{
//	QFileInfo fi(filename);
//	bool bNextWad = false; // if set to false it will perform WWW::checkNextSite().
//
//	QString path = this->targetDir + currentWad;
//	if (filename.compare(currentWad, Qt::CaseInsensitive) == 0)
//	{
//		QFile f(path);
//		if (!f.open(QIODevice::WriteOnly))
//		{
//			emit message(tr("Failed to save file: %1").arg(path), WadseekerLib::CriticalError);
//			return;
//		}
//
//		int writeLen = f.write(data);
//		f.close();
//
//		if (writeLen != data.length())
//		{
//			emit message(tr("Failed to save file: %1").arg(path), WadseekerLib::CriticalError);
//			return;
//		}
//
//		bNextWad = true;
//	}
//	else
//	{
//		UnArchive *unarchive = UnArchive::OpenArchive(fi, data);
//
//		if (unarchive == NULL || !unarchive->isValid())
//		{
//			emit message(tr("Couldn't unarchive \"%1\".").arg(filename), WadseekerLib::Error);
//		}
//		else
//		{
//			connect (unarchive, SIGNAL( message(const QString&, int) ), this, SLOT( messageSlot(const QString&, int) ) );
//			// Check the archive for any of the files we're trying to locate.
//			QStack<int> filesToExtract;
//			int file;
//			if ((file = unarchive->findFileEntry(currentWad)) != -1)
//			{
//                filesToExtract.push(file);
//            }
//
//			for (int i = iNextWad;i < seekedWads.size();i++)
//			{
//				if ((file = unarchive->findFileEntry(seekedWads[i])) != -1)
//				{
//					seekedWads[i] = QString();
//					filesToExtract.push(file);
//				}
//			}
//
//			if(!filesToExtract.isEmpty())
//			{
//				do
//				{
//					file = filesToExtract.pop();
//					QString extractedFileName = unarchive->fileNameFromIndex(file);
//
//					unarchive->extract(file, path);
//					emit message(tr("File \"%1\" was uncompressed successfully from archive \"%2\"!").arg(extractedFileName, filename), WadseekerLib::Notice);
//					bNextWad = true;
//				}
//				while(!filesToExtract.isEmpty());
//			}
//			else
//			{
//				emit message(tr("File \"%1\" not found in \"%2\"").arg(currentWad, filename), WadseekerLib::Error);
//			}
//		}
//		delete unarchive;
//	}
//
//	if (bNextWad)
//	{
//		nextWad();
//	}
//	else
//	{
//		www->checkNextSite();
//	}
//}

void Wadseeker::fileLinkFound(const QString& filename, const QUrl& url)
{
	emit message(tr("Found link to file \"%1\": %2").arg(filename).arg(url.toEncoded().constData()), WadseekerLib::Notice);
}

bool Wadseeker::isAllFinished() const
{
	return !d.wwwSeeker->isWorking();
}

bool Wadseeker::isForbiddenWad(const QString& wad)
{
	QFileInfo fiWad(wad);
	// Check the basename, ignore extension.
	// This will block names like "doom2.zip" but also "doom2.pk3" and
	// "doom2.whatever".
	QString basename = fiWad.completeBaseName();
	for (int i = 0; !forbiddenWads[i].isEmpty(); ++i)
	{
		if(basename.compare(forbiddenWads[i], Qt::CaseInsensitive) == 0)
		{
			return true;
		}
	}

	return false;
}

bool Wadseeker::isWorking() const
{
	return d.wwwSeeker->isWorking();
}

void Wadseeker::setCustomSite(const QUrl& url)
{
	d.seekParameters.customSiteUrl = url;
}

void Wadseeker::setIdgamesEnabled(bool bEnabled)
{
	d.seekParameters.bIdgamesEnabled = bEnabled;
}

void Wadseeker::setIdgamesHighPriority(bool bHighPriority)
{
	d.seekParameters.bIdgamesHighPriority = bHighPriority;
}

void Wadseeker::setIdgamesUrl(QString archiveUrl)
{
	d.seekParameters.idgamesUrl = archiveUrl;
}

void Wadseeker::setPrimarySites(const QStringList& urlList)
{
	d.seekParameters.sitesUrls = urlList;
}

void Wadseeker::setPrimarySitesToDefault()
{
	QList<QString> list;
	for (int i = 0; !defaultSites[i].isEmpty(); ++i)
	{
		list << defaultSites[i];
	}
	setPrimarySites(list);
}

void Wadseeker::setTargetDirectory(const QString& dir)
{
	QString targetDir = dir;
	if (!dir.isEmpty())
	{
		if (dir[dir.length() - 1] != QDir::separator())
		{
			targetDir += '/';
		}
	}

	d.seekParameters.saveDirectoryPath = targetDir;
}

void Wadseeker::setupSitesUrls()
{
	d.wwwSeeker->clearVisitedUrlsList();

	if (d.seekParametersForCurrentSeek->customSiteUrl.isValid())
	{
		d.wwwSeeker->addSiteUrl(d.seekParametersForCurrentSeek->customSiteUrl);
	}

	foreach (const QString& strSiteUrl, d.seekParametersForCurrentSeek->sitesUrls)
	{
		// If URL containts %WADNAME% placeholder we need to create a unique
		// URL for each searched wad.
		if (strSiteUrl.contains("%WADNAME%"))
		{
			foreach (const QString& wad, d.seekParametersForCurrentSeek->seekedWads)
			{
				QString strProperUrl = strSiteUrl;
				strProperUrl.replace("%WADNAME%", wad);

				QUrl url(strProperUrl);
				if (url.isValid())
				{
					d.wwwSeeker->addFileSiteUrl(wad, url);
				}
			}
		}
		else
		{
			QUrl url(strSiteUrl);
			if (url.isValid())
			{
				d.wwwSeeker->addSiteUrl(url);
			}
		}
	}
}

bool Wadseeker::startSeek(const QStringList& wads)
{
	if (d.seekParametersForCurrentSeek != NULL)
	{
		emit message(tr("Seek already in progress. Please abort the current seek before starting a new one."),
					WadseekerLib::CriticalError);
		return false;
	}

	if (d.seekParameters.saveDirectoryPath.isEmpty())
	{
		QString err = tr("No target directory specified! Aborting.");
		emit message(err, WadseekerLib::CriticalError);
		return false;
	}

	QDir targetDir(d.seekParameters.saveDirectoryPath);
	if (!targetDir.exists())
	{
		emit message(tr("File save directory \"%1\" doesn't exist or is a file. Aborting.").arg(d.seekParameters.saveDirectoryPath),
					WadseekerLib::CriticalError);
		return false;
	}

	QStringList filteredWadsList;
	foreach (QString wad, wads)
	{
		wad = wad.trimmed();
		if (wad.isEmpty())
		{
			continue;
		}

		if (isForbiddenWad(wad))
		{
			emit message(tr("WAD \"%1\" is on the forbidden list. Wadseeker will not download this WAD.").arg(wad),
				WadseekerLib::Error);
			continue;
		}

		filteredWadsList << wad;
	}

	if (filteredWadsList.isEmpty())
	{
		emit message(tr("Cannot start search. No WADs to seek."),
			WadseekerLib::CriticalError);
		return false;
	}

	d.seekParameters.seekedWads = filteredWadsList;
	d.seekParametersForCurrentSeek = new SeekParameters(d.seekParameters);

	setupSitesUrls();

	QList<FileSeekInfo> fileSeekInfosList;

	foreach (const QString& wad, filteredWadsList)
	{
		// Create download info object for this WAD.
		WadDownloadInfo wadDownloadInfo(wad);

		// TODO
		// Store the object in order to keep download status.

		// Generate all possible filenames.
		QStringList possibleFilenames;
		possibleFilenames << wadDownloadInfo.possibleArchiveNames();
		possibleFilenames << wadDownloadInfo.possibleWadNames();
		possibleFilenames << wad;

		fileSeekInfosList << FileSeekInfo(wad, possibleFilenames);
	}

	emit seekStarted(filteredWadsList);
	d.wwwSeeker->startSearch(fileSeekInfosList);

	return true;
}

QString Wadseeker::targetDirectory() const
{
	return d.seekParameters.saveDirectoryPath;
}

QStringList Wadseeker::wantedFilenames(const QString& wadName, QString& zipName)
{
	// TODO
	// Use supported archives list here.

	QStringList list;
	list.append(wadName);

	QFileInfo fi(wadName);
	if (fi.suffix().compare("7z", Qt::CaseInsensitive) != 0)
	{
		QString archiveName = fi.completeBaseName() + ".7z";
		list.append(archiveName);
	}

	if (fi.suffix().compare("zip", Qt::CaseInsensitive) != 0)
	{
		QString archiveName = fi.completeBaseName() + ".zip";
		zipName = archiveName;
		list.append(archiveName);
	}
	else
	{
		zipName = wadName;
	}

	return list;
}

void Wadseeker::wwwSeekerFinished()
{
	if (isAllFinished())
	{
		cleanUpAfterFinish();
	}
}

////////////////////////////////////////////////////////////////////////////////

Wadseeker::SeekParameters::SeekParameters()
{
	this->bIdgamesEnabled = true;
	this->bIdgamesHighPriority = true;
	this->idgamesUrl = Wadseeker::defaultIdgamesUrl();
	this->sitesUrls = Wadseeker::defaultSitesListEncoded();
}
