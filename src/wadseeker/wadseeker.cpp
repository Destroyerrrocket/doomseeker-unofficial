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
#include <QFileInfo>
#include <QTemporaryFile>

QString Wadseeker::defaultSites[] =
{
	QString("http://raider.dnsalias.com:8001/doom/userwads/"),
	QString("http://doom.dogsoft.net/getwad.php?search=%WADNAME%"),
	QString("http://hs.keystone.gr/lap/"),
	QString("http://www.rarefiles.com/download/"),
	QString("http://supergod.servegame.com/"),
	QString("") // empty url is treated here like an '\0' in a string
};

QString Wadseeker::iwadNames[] =
{
	"doom", "doom2", "heretic", "hexen", "tnt", "plutonia", "hexdd", "strife1", "voices", ""
};

QStringList Wadseeker::defaultSitesListEncoded()
{
	QStringList list;
	for (int i = 0; !defaultSites[i].isEmpty(); ++i)
	{
		list << QUrl::toPercentEncoding(defaultSites[i]);
	}
	return list;
}

Wadseeker::Wadseeker()
{
	connect(&www, SIGNAL( dataReceived(unsigned, unsigned, unsigned) ), this, SLOT( sizeUpdate(unsigned, unsigned, unsigned) ) );
	connect(&www, SIGNAL( error(const QString&) ), this, SLOT( wwwError(const QString&) ) );
	connect(&www, SIGNAL( finishedReceiving(const QByteArray&) ), this, SLOT( finishedReceiving(const QByteArray&) ) );
	connect(&www, SIGNAL( notice(const QString&) ), this, SLOT( wwwNotice(const QString&) ) );
	connect(&www, SIGNAL( noMoreSites() ), this, SLOT( wwwNoMoreSites() ) );
	connect(&www, SIGNAL( size(unsigned int) ), this, SLOT( size(unsigned int) ) );

	connect(this, SIGNAL( wadDone(bool, const QString&) ), this, SLOT( wadDoneSlot(bool, const QString&) ));
}

Wadseeker::~Wadseeker()
{
}

void Wadseeker::abort()
{
	www.abort();
}

void Wadseeker::finishedReceiving(const QByteArray& data)
{
	PARSE_FILE_RETURN_CODES ret = this->parseFile(data);
	switch(ret)
	{
		case PARSE_FILE_CRITICAL_ERROR:
			return;

		case PARSE_FILE_ERROR:
			www.nextSite();
			break;

		case PARSE_FILE_OK:
			emit wadDone(true, seekedWad);
			this->seekNextWad();
			break;
	}
}

bool Wadseeker::isIwad(const QString& wad)
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



QString Wadseeker::nextWadName()
{
	if (currentWad == wadnames.end())
	{
		return QString();
	}

	QString str = *currentWad;
	++currentWad;
	return str;
}

Wadseeker::PARSE_FILE_RETURN_CODES Wadseeker::parseFile(const QByteArray& data)
{
	// first we check if the seeked file and retrieved file have exactly the same filename
	QFileInfo fi(www.lastUrl().path());
	QString filename = fi.fileName();

	// If they do have the same name we simply dump the file into directory and return OK.
	if (filename.compare(seekedWad, Qt::CaseInsensitive) == 0)
	{
		QString path = this->targetDirectory + filename;
		QFile f(path);
		if (!f.open(QIODevice::WriteOnly))
		{
			emit error(tr("Failed to save file: %1").arg(path), true);
			return PARSE_FILE_CRITICAL_ERROR;
		}

		int writeLen = f.write(data);
		f.close();

		if (writeLen != data.length())
		{
			emit error(tr("Failed to save file: %1").arg(path), true);
			return PARSE_FILE_CRITICAL_ERROR;
		}

		return PARSE_FILE_OK;
	}
	else if (fi.suffix().compare("zip", Qt::CaseInsensitive) == 0)
	{
		return parseZipFile(data);
	}
	else
	{
		emit error(tr("File %1 failed (on site %2)").arg(seekedWad, www.lastUrl().toString()) , false);
		return PARSE_FILE_ERROR;
	}
}

Wadseeker::PARSE_FILE_RETURN_CODES Wadseeker::parseZipFile(const QByteArray& data)
{
	QFileInfo fi(www.lastUrl().path());
	QString filename = fi.fileName();
	QString path = this->targetDirectory + seekedWad;

	// Open temporary file
	QTemporaryFile tempFile;

	tempFile.open();
	QString tempFileName = tempFile.fileName();

	QFileInfo fiTemp(tempFileName);
	if (!fiTemp.exists())
	{
		emit error(tr("Couldn't create temporary file to unzip \"%1\".").arg(filename), false);
		return PARSE_FILE_ERROR;
	}

	tempFile.setAutoRemove(false);
	tempFile.write(data);
	tempFile.close();

	// Unzip temporary file
	int err = unzipFile(tempFileName, filename, path);

	tempFile.remove();
	return static_cast<PARSE_FILE_RETURN_CODES>(err);
}

void Wadseeker::seekNextWad()
{
	QString str = nextWadName();
	if (!str.isEmpty())
	{
		seekWad(str);
	}
	else
	{
		emit allDone();
	}
}

void Wadseeker::seekWad(const QString& wad)
{
	QString notic = tr("Seeking file: %1").arg(wad);
	emit notice(notic);
	if (!this->isIwad(wad))
	{
		seekedWad = wad;
		www.get(wad); // get wad :P
	}
	else
	{
		emit error(tr("This is an IWAD!"), false);
		emit wadDone(false, wad);
		this->seekNextWad();
	}
}

void Wadseeker::seekWads(const QStringList& wads)
{
	wadnames = wads;
	currentWad = wadnames.begin();
	notFoundWads = wads;

	if (targetDirectory.isEmpty())
	{
		QString err = tr("No target directory specified! Aborting");
		emit error(err, true);
		return;
	}

	QFileInfo fi(targetDirectory);
	fi.isWritable();

	if (!fi.exists() || !fi.isDir())
	{
		QString err = tr("Target directory: \"%1\" doesn't exist! Aborting").arg(targetDirectory);
		emit error(err, true);
		return;
	}

	if (!fi.isWritable())
	{
		QString err = tr("You cannot write to directory: \"%1\"! Aborting").arg(targetDirectory);
		emit error(err, true);
		return;
	}

	seekNextWad();
}

void Wadseeker::setGlobalSiteLinksToDefaults()
{
	QList<QString> list;
	for (int i = 0; !defaultSites[i].isEmpty(); ++i)
	{
		list << defaultSites[i];
	}
	setGlobalSiteLinks(list);
}

void Wadseeker::size(unsigned int s)
{
	emit wadSize(s);
}

void Wadseeker::sizeUpdate(unsigned howMuch, unsigned howMuchSum, unsigned percent)
{
	emit wadCurrentDownloadedSize(howMuchSum, percent);
}

void Wadseeker::wadDoneSlot(bool bFound, const QString& wadname)
{
	// if wad was found remove it from not found list
	if (bFound)
	{
		QStringList::iterator it;
		for (it = notFoundWads.begin(); it != notFoundWads.end(); ++it)
		{
			if (it->compare(wadname, Qt::CaseInsensitive) == 0)
			{
				notFoundWads.erase(it);
				break;
			}
		}
	}
}

Wadseeker::PARSE_FILE_RETURN_CODES Wadseeker::unzipFile(const QString& zipFileName, const QString& displayFileName, const QString& targetPath)
{
	UnZip unzip(zipFileName);
	if (!unzip.isValid())
	{
		emit error(tr("Couldn't open \"%1\" to unzip \"%2\".").arg(zipFileName, displayFileName), false);
		return PARSE_FILE_ERROR;
	}

	connect (&unzip, SIGNAL( error(const QString&) ), this, SLOT( zipError(const QString&) ) );
	connect (&unzip, SIGNAL( notice(const QString&) ), this, SLOT( zipNotice(const QString&) ) );
	ZipLocalFileHeader* zip = unzip.findFileEntry(seekedWad);
	if (zip != NULL)
	{
		unzip.extract(*zip, targetPath);
		delete zip;
	}
	else
	{
		emit error(tr("File \"%1\" not found in \"%2\"").arg(seekedWad, displayFileName), false);
		return PARSE_FILE_ERROR;
	}
}

void Wadseeker::wwwError(const QString& errorString)
{
	emit error(errorString, false);
}

void Wadseeker::wwwNoMoreSites()
{
	emit notice(tr("No more sites!"));
	emit wadDone(false, seekedWad);
	this->seekNextWad();
}

void Wadseeker::wwwNotice(const QString& string)
{
	emit notice(string);
}

void Wadseeker::zipError(const QString& str)
{
	emit error(tr("UnZip error: %1").arg(str), false);
}

void Wadseeker::zipNotice(const QString& str)
{
	emit notice(tr("UnZip: %1").arg(str));
}
