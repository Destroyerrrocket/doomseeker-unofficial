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
#include "wwwseeker.h"
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
	this->seekParametersForCurrentSeek = NULL;
}

Wadseeker::~Wadseeker()
{
}

void Wadseeker::abort()
{

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

void Wadseeker::startSeek(const QStringList& wads)
{
	if (seekParametersForCurrentSeek != NULL)
	{
		emit message(tr("Seek already in progress. Please abort the current seek before starting a new one."),
					WadseekerLib::CriticalError);
		return;
	}

	if (wads.isEmpty())
	{
		emit message(tr("Specified no files to seek. Aborting."), WadseekerLib::CriticalError);
		return;
	}

	if (seekParameters.saveDirectoryPath.isEmpty())
	{
		QString err = tr("No target directory specified! Aborting.");
		emit message(err, WadseekerLib::CriticalError);
		return;
	}

	QDir targetDir(seekParameters.saveDirectoryPath);
	if (!targetDir.exists())
	{
		emit message(tr("File save directory \"%1\" doesn't exist or is a file. Aborting.").arg(seekParameters.saveDirectoryPath),
					WadseekerLib::CriticalError);
		return;
	}

	seekParameters.seekedWads = wads;
	seekParametersForCurrentSeek = new SeekParameters(seekParameters);
	emit seekStarted(wads);
}


void Wadseeker::setCustomSite(const QUrl& url)
{
	seekParameters.customSiteUrl = url;
}

void Wadseeker::setIdgamesEnabled(bool bEnabled)
{
	seekParameters.bIdgamesEnabled = bEnabled;
}

void Wadseeker::setIdgamesHighPriority(bool bHighPriority)
{
	seekParameters.bIdgamesHighPriority = bHighPriority;
}

void Wadseeker::setIdgamesUrl(QString archiveUrl)
{
	seekParameters.idgamesUrl = archiveUrl;
}

void Wadseeker::setPrimarySites(const QStringList& urlList)
{
	seekParameters.sitesUrls = urlList;
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

	seekParameters.saveDirectoryPath = targetDir;
}

QString Wadseeker::targetDirectory() const
{
	return seekParameters.saveDirectoryPath;
}

QStringList Wadseeker::wantedFilenames(const QString& wadName, QString& zipName)
{
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

////////////////////////////////////////////////////////////////////////////////

Wadseeker::SeekParameters::SeekParameters()
{
	this->bIdgamesEnabled = true;
	this->bIdgamesHighPriority = true;
	this->idgamesUrl = Wadseeker::defaultIdgamesUrl();
	this->sitesUrls = Wadseeker::defaultSitesListEncoded();
}
