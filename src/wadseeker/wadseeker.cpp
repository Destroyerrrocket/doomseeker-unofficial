//------------------------------------------------------------------------------
// wadseeker.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "wadseeker.h"
#include "www.h"
#include "zip/unzip.h"
#include <QDir>
#include <QFileInfo>

const QString Wadseeker::defaultSites[] =
{
	QString("http://raider.dnsalias.com:8001/doom/userwads/"),
	QString("http://doom.dogsoft.net/getwad.php?search=%WADNAME%"),
	QString("http://hs.keystone.gr/lap/"),
	QString("http://www.rarefiles.com/download/"),
	QString("http://supergod.servegame.com/"),
	QString("http://www.coffeenet.org/~vulture/wads/"),
	QString("") // empty url is treated here like an '\0' in a string
};

const QString Wadseeker::iwadNames[] =
{
	"doom", "doom2", "heretic", "hexen", "tnt", "plutonia", "hexdd", "strife1", "voices", ""
};
///////////////////////////////////////////////////////////////////////
Wadseeker::Wadseeker()
{
	www = new WWW();

	connect(www, SIGNAL( downloadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(www, SIGNAL( fileDone(QByteArray&, const QString&) ), this, SLOT( fileDone(QByteArray&, const QString&) ));
	connect(www, SIGNAL( message(const QString&, Wadseeker::MessageType) ), this, SLOT( messageSlot(const QString&, Wadseeker::MessageType) ) );
	connect(www, SIGNAL( noMoreSites() ), this, SLOT( wadFail() ) );
}

Wadseeker::~Wadseeker()
{
	delete www;
}

void Wadseeker::abort()
{
	www->abort();
	for (int i = iNextWad - 1; i < seekedWads.size(); ++i)
	{
		notFound.append(seekedWads[i]);
	}
	emit aborted();
}

bool Wadseeker::areAllFilesFound() const
{
	return notFound.isEmpty();
}

const QStringList& Wadseeker::filesNotFound() const
{
	return notFound;
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
	emit downloadProgress(done, total);
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
	else if (fi.suffix().compare("zip", Qt::CaseInsensitive) == 0)
	{
		UnZip unzip(data);
		if (!unzip.isValid())
		{
			emit message(tr("Couldn't unzip \"%1\".").arg(filename), Error);
		}
		else if (!unzip.isZip())
		{
			emit message(tr("\"%1\" is not a valid ZIP file.").arg(filename), Error);
		}
		else
		{
			connect (&unzip, SIGNAL( message(const QString&, Wadseeker::MessageType) ), this, SLOT( messageSlot(const QString&, Wadseeker::MessageType) ) );
			ZipLocalFileHeader* zip = unzip.findFileEntry(currentWad);

			if (zip != NULL)
			{
				unzip.extract(*zip, path);
				emit message(tr("%1#%2 uncompressed successfuly!").arg(filename, zip->fileName), Notice);
				delete zip;
				bNextWad = true;
			}
			else
			{
				emit message(tr("File \"%1\" not found in \"%2\"").arg(currentWad, filename), Error);
			}
		}
	}

	emit message(" ", Notice);
	if (bNextWad)
	{
		nextWad();
	}
	else
	{
		www->checkNextSite();
	}
}

bool Wadseeker::isIwad(const QString& wad) const
{
	QFileInfo fiWad(wad);
	// Check the basename, ignore extension.
	// This will block names like "doom2.zip" but also "doom2.pk3" and
	// "doom2.whatever".
	QString basename = fiWad.completeBaseName();
	for (int i = 0; !iwadNames[i].isEmpty(); ++i)
	{
		if(basename.compare(iwadNames[i], Qt::CaseInsensitive) == 0)
		{
			return true;
		}
	}

	return false;
}

void Wadseeker::messageSlot(const QString& msg, MessageType type)
{
	emit message(msg, type);
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

		if (isIwad(wad))
		{
			emit message(tr("%1 is an IWAD. Ignoring.\n").arg(wad), Error);
			notFound.append(wad);
			wad = QString();
		}
	}

	emit message(tr("Seeking file: %1").arg(wad), Notice);
	currentWad = wad;
	QStringList wfi = wantedFilenames(wad);
	www->searchFiles(wfi, currentWad);
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

QStringList	Wadseeker::wantedFilenames(const QString& wad) const
{
	QStringList lst;
	lst.append(wad);

	QFileInfo fi(wad);
	if (fi.suffix().compare("zip", Qt::CaseInsensitive) != 0)
	{
		QString app = fi.completeBaseName() + ".zip";
		lst.append(app);
	}

	return lst;
}
