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
#include "zip/unzip.h"
#include "zip/un7zip.h"
#include "speedcalculator.h"
#include "wadseeker.h"
#include "wadseekerversioninfo.h"
#include "www.h"
#include <QDir>
#include <QFileInfo>

// TODO:
// For the (close?) future:
//
// Sometimes two or more WADs that are being hosted on a server are distributed
// within the same archive. Because of this Wadseeker should keep a cache of 
// downloaded archives during it's search and try to retrieve subsequent 
// wads from these archives. This should also work backwards: if we are looking
// for 3 wads and the last one gets found inside an archive while the others 
// are missing we check this archive for the previous files.
//
// We're not concerned about memory issues here. It's perfectly fine to store
// a 100MB archive in memory... and if it's not we can use temp files that 
// will clutter the system and fill out entire disk space upon application crash
// eventually!
//
// PS. Maybe saving the archives to disk should also be allowed?
//

const QString Wadseeker::defaultSites[] =
{
	QString("http://doom.dogsoft.net/getwad.php?search=%WADNAME%"),
	QString("http://www.wadhost.fathax.com/files/"),
	QString("http://hs.keystone.gr/lap/"),
	QString("http://www.rarefiles.com/download/"),
	QString("http://wads.coffeenet.org/zips/"),
	QString("http://wads.interdoom.com/"),
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
	speedCalculator = new SpeedCalculator();
	www = new WWWSeeker();

	www->setUserAgent("Wadseeker/" + WadseekerVersionInfo::version());

	connect(www, SIGNAL( aborted() ), this, SLOT( wwwAborted() ) );
	connect(www, SIGNAL( downloadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(www, SIGNAL( fileDone(QByteArray&, const QString&) ), this, SLOT( fileDone(QByteArray&, const QString&) ));
	connect(www, SIGNAL( message(const QString&, int) ), this, SLOT( messageSlot(const QString&, int) ) );
	connect(www, SIGNAL( fail() ), this, SLOT( wadFail() ) );
}

Wadseeker::~Wadseeker()
{
	delete speedCalculator;
	delete www;
}

void Wadseeker::abort()
{
	www->abort();
	for (int i = iNextWad - 1; i < seekedWads.size(); ++i)
	{
		notFound.append(seekedWads[i]);
	}
}

bool Wadseeker::areAllFilesFound() const
{
	return notFound.isEmpty();
}

const QStringList& Wadseeker::filesNotFound() const
{
	return notFound;
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

void Wadseeker::downloadProgressSlot(int done, int total)
{
	// There's no performance drop or any other argument against setting
	// expected data size each time this slot is called.
	// It's stupid but it's a simple solution.
	speedCalculator->setExpectedDataSize(total);
	speedCalculator->registerDataAmount(done);	
	emit downloadProgress(done, total);
}

float Wadseeker::downloadSpeed() const
{
	return speedCalculator->getSpeed();
}

float Wadseeker::estimatedTimeUntilArrivalOfCurrentFile() const
{
	return speedCalculator->estimatedTimeUntilArrival();
}

void Wadseeker::fileDone(QByteArray& data, const QString& filename)
{
	QFileInfo fi(filename);
	bool bNextWad = false; // if set to false it will perform WWW::checkNextSite().

	QString path = this->targetDir + currentWad;
	if (filename.compare(currentWad, Qt::CaseInsensitive) == 0)
	{
		QFile f(path);
		if (!f.open(QIODevice::WriteOnly))
		{
			emit message(tr("Failed to save file: %1").arg(path), CriticalError);
			return;
		}

		int writeLen = f.write(data);
		f.close();

		if (writeLen != data.length())
		{
			emit message(tr("Failed to save file: %1").arg(path), CriticalError);
			return;
		}

		bNextWad = true;
	}
	else if (fi.suffix().compare("zip", Qt::CaseInsensitive) == 0 || fi.suffix().compare("7z", Qt::CaseInsensitive) == 0)
	{
		UnArchive *unarchive = NULL;
		if (fi.suffix().compare("zip", Qt::CaseInsensitive) == 0)
		{
			unarchive = new UnZip(data);
		}
		else
		{
			unarchive = new Un7Zip(data);
		}

		if (!unarchive->isValid())
		{
			emit message(tr("Couldn't unarchive \"%1\".").arg(filename), Error);
		}
		else
		{
			connect (unarchive, SIGNAL( message(const QString&, int) ), this, SLOT( messageSlot(const QString&, int) ) );
			int file = unarchive->findFileEntry(currentWad);

			if (file != -1)
			{
				QString extractedFileName = unarchive->fileNameFromIndex(file);
			
				unarchive->extract(file, path);
				emit message(tr("File \"%1\" was uncompressed successfully from archive \"%2\"!").arg(extractedFileName, filename), Notice);
				bNextWad = true;
			}
			else
			{
				emit message(tr("File \"%1\" not found in \"%2\"").arg(currentWad, filename), Error);
			}
		}
		delete unarchive;
	}

	if (bNextWad)
	{
		nextWad();
	}
	else
	{
		www->checkNextSite();
	}
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

void Wadseeker::messageSlot(const QString& msg, int type)
{
	emit message(msg, static_cast<Wadseeker::MessageType>(type));
}

void Wadseeker::nextWad()
{
	QString wad;
	while (wad.isNull() || wad.isEmpty())
	{
		if (iNextWad >= seekedWads.size())
		{
			emit allDone();
			return;
		}

		wad = seekedWads[iNextWad];
		++iNextWad;

		if (isForbiddenWad(wad))
		{
			emit message(tr("%1 is an IWAD or commercial mod. Ignoring.").arg(wad), Error);
			notFound.append(wad);
			wad = QString();
		}
	}

	emit message(tr("Seeking file: %1").arg(wad), Notice);
	
	currentWad = wad;
	QString zipName;
	QStringList wantedFilenamesInfo = wantedFilenames(wad, zipName);
	
	speedCalculator->start();
	www->searchFiles(wantedFilenamesInfo, currentWad, zipName);
}

void Wadseeker::seekWads(const QStringList& wads)
{
	iNextWad = 0;
	notFound.clear();
	seekedWads = wads;

	if (wads.isEmpty())
		return;

	if (targetDir.isEmpty())
	{
		QString err = tr("No target directory specified! Aborting");
		emit message(err, CriticalError);
		return;
	}

	QFileInfo fi(targetDir);
	fi.isWritable();

	if (!fi.exists() || !fi.isDir())
	{
		QString err = tr("Target directory: \"%1\" doesn't exist! Aborting").arg(targetDir);
		emit message(err, CriticalError);
		return;
	}

	if (!fi.isWritable())
	{
		QString err = tr("You cannot write to directory: \"%1\"! Aborting").arg(targetDir);
		emit message(err, CriticalError);
		return;
	}

	nextWad();
}


void Wadseeker::setCustomSite(const QUrl& url)
{
	www->setCustomSite(url);
}

void Wadseeker::setPrimarySites(const QStringList& lst)
{
	www->setPrimarySites(lst);
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
	targetDir = dir;
	if (!dir.isEmpty())
	{
		if (dir[dir.length() - 1] != QDir::separator())
			targetDir += '/';
	}
}

void Wadseeker::setUseIdgames(bool use, bool highPriority, QString archiveURL)
{
	www->setUseIdgames(use, highPriority, archiveURL);
}

void Wadseeker::setTimeConnectTimeout(int i)
{
	WWW::setTimeConnectTimeout(i);
}

void Wadseeker::setTimeDownloadTimeout(int i)
{
	WWW::setTimeDownloadTimeout(i);
}

void Wadseeker::skipSite()
{
	www->skipSite();
}

QString Wadseeker::targetDirectory() const
{
	return targetDir;
}

void Wadseeker::wadFail()
{
	QString tmp = tr("%1 WAS NOT FOUND\n").arg(seekedWads[iNextWad - 1]);
	notFound.append(seekedWads[iNextWad - 1]);
	emit message(tmp, Error);
	emit downloadProgress(100, 100);
	nextWad();
}

QStringList Wadseeker::wantedFilenames(const QString& wad, QString& zip)
{
	QStringList lst;
	lst.append(wad);

	QFileInfo fi(wad);
	if (fi.suffix().compare("7z", Qt::CaseInsensitive) != 0)
	{
		QString app = fi.completeBaseName() + ".7z";
		lst.append(app);
	}

	if (fi.suffix().compare("zip", Qt::CaseInsensitive) != 0)
	{
		QString app = fi.completeBaseName() + ".zip";
		zip = app;
		lst.append(app);
	}
	else
	{
		zip = wad;
	}

	return lst;
}

void Wadseeker::wwwAborted()
{
	emit aborted();
}
