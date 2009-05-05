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

QUrl Wadseeker::globalSiteLinks[] =
{
	QUrl("http://zalewa.dyndns.org/robert/wadseeker_test/test2.html"),
	QUrl("http://supergod.servegame.com/"),
	QUrl("") // empty url is treated here like an '\0' in a string
};

QString Wadseeker::iwadNames[] =
{
	"doom", "doom2", "heretic", "hexen", "tnt", "plutonia", "hexdd", "strife", ""
};

Wadseeker::Wadseeker()
{
	currentGlobalSite = 0;

	connect(&http, SIGNAL( dataReceived(unsigned, unsigned, unsigned) ), this, SLOT( sizeUpdate(unsigned, unsigned, unsigned) ) );
	connect(&http, SIGNAL( error(const QString&) ), this, SLOT( httpError(const QString&) ) );
	connect(&http, SIGNAL( finishedReceiving(const QString&) ), this, SLOT( finishedReceiving(const QString&) ) );
	connect(&http, SIGNAL( size(unsigned int) ), this, SLOT( size(unsigned int) ) );
	//connect(this, SIGNAL( wadDone(bool, const QString&) ), this, SLOT( seekNextWad(bool, const QString&) ) );
}

Wadseeker::~Wadseeker()
{
}

void Wadseeker::abort()
{
	http.abort();
}

void Wadseeker::finishedReceiving(const QString& err)
{
	if (!err.isEmpty())
	{
		QString str = tr("HTTP error: ") + err;
		emit error(str, true);
		return;
	}

	if( http.lastFileType() == Http::HTTP_FILE_TYPE_BINARY)
	{
		PARSE_FILE_RETURN_CODES ret = this->parseFile();
		switch(ret)
		{
			case PARSE_FILE_CRITICAL_ERROR:
				return;

			case PARSE_FILE_ERROR:
				this->nextSite();
				break;

			case PARSE_FILE_OK:
				emit wadDone(true, seekedWad);
				this->seekNextWad();
				break;
		}
	}
	else if ( http.lastFileType() == Http::HTTP_FILE_TYPE_HTML)
	{
		this->getLinks();
		this->nextSite();
	}

}

bool Wadseeker::hasFileReferenceSomewhere(const QStringList& wantedFileNames, const Link& link)
{
	for (int i = 0; i < wantedFileNames.count(); ++i)
	{
		if (link.url.toString().contains(wantedFileNames[i], Qt::CaseInsensitive) || link.text.contains(wantedFileNames[i], Qt::CaseInsensitive) )
		{
			return true;
		}
	}

	return false;
}

void Wadseeker::httpError(const QString& errorString)
{
	QString str = tr("Http error: ") + errorString;
	emit error(str, false);
	nextSite();
}

void Wadseeker::getLinks()
{
	QFileInfo fi(seekedWad);
	QString extension = fi.suffix();

	QStringList wantedFileNames;
	wantedFileNames << fi.completeBaseName() + ".zip";
	if (extension.compare("zip", Qt::CaseInsensitive) != 0)
	{
		wantedFileNames << fi.fileName();
	}

	QList<Link> list = http.links();
	QList<Link>::iterator it;

	for (it = list.begin(); it != list.end(); ++it)
	{
		if (it->isHttpLink())
		{
			QString strUrl;
			if (it->url.authority().isEmpty())
			{
				QList<QByteArray> path = url.encodedPath().split('/');

				strUrl = "http://" + url.authority();
				if (strUrl[strUrl.length() - 1] != '/')
					strUrl += '/';

				for (int i = 0; i < path.length() - 1; ++i)
				{
					if (!path[i].isEmpty())
					{
						strUrl += path[i] + '/';
					}
				}
			}
			else
			{
				strUrl = "";
			}

			QUrl newUrl(strUrl + it->url.toString());

			if (isDirectLinkToFile(wantedFileNames, it->url))
			{
				directLinks.append(newUrl);
			}
			else if (hasFileReferenceSomewhere(wantedFileNames, *it))
			{
				// here we append all links that contain this filename somewhere else than in path
				siteLinks.append(newUrl);
			}
		}
	}
}

bool Wadseeker::isDirectLinkToFile(const QStringList& wantedFileNames, const QUrl& link)
{
	QFileInfo fi(link.encodedPath());
	for (int i = 0; i < wantedFileNames.count(); ++i)
	{
		if (fi.fileName() == wantedFileNames[i])
		{
			return true;
		}
	}

	return false;
}

bool Wadseeker::isIwad(const QString& wad)
{
	QFileInfo fiWad(wad);
	// Check the basename, ignore extension.
	// This will block names like "doom2.zip" but also "doom2.pk3" and
	// "doom2.whatever".
	QString basename = fiWad.baseName();
	for (int i = 0; !iwadNames[i].isEmpty(); ++i)
	{
		if(basename.compare(iwadNames[i], Qt::CaseInsensitive) == 0)
		{
			return true;
		}
	}

	return false;
}

void Wadseeker::nextSite()
{
	bool bGotUrl = false;

	if (!customSiteUsed && !customSite.isEmpty())
	{
		url = customSite;
		bGotUrl = true;
		customSiteUsed = true;
	}

	while (!directLinks.empty() && !bGotUrl)
	{
		url = directLinks.first();
		directLinks.removeFirst();
		if (checkedLinks.find(url.toString()) == checkedLinks.end())
		{
			bGotUrl = true;
		}
	}

	while (!siteLinks.empty() && !bGotUrl)
	{
		url = siteLinks.first();
		siteLinks.removeFirst();
		if (checkedLinks.find(url.toString()) == checkedLinks.end())
		{
			bGotUrl = true;
		}
	}

	if (!bGotUrl)
	{
		url = globalSiteLinks[currentGlobalSite];
		if (!url.isEmpty())
		{
			++currentGlobalSite;
			bGotUrl = true;
		}
	}

	if (!bGotUrl)
	{
		emit notice(tr("No more sites!"));
		emit wadDone(false, seekedWad);
		this->seekNextWad();
		return;
	}

	checkedLinks.insert(url.toString());

	QString strNotice = tr("Next site: ") + url.toString();
	emit notice(strNotice);

	http.setSite(url.encodedHost());
	http.sendRequestGet(url.encodedPath());
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

Wadseeker::PARSE_FILE_RETURN_CODES Wadseeker::parseFile()
{
	// first we check if the seeked file and retrieved file have exactly the same filename
	QFileInfo fi(http.lastLink().path());
	QString filename = fi.fileName();

	// If they do have the same name we simply dump the file into directory and return OK.
	if (filename.compare(seekedWad, Qt::CaseInsensitive) == 0)
	{
		QByteArray& data = http.lastData();
		QString path = this->targetDirectory + filename;
		QFile f(path);
		if (!f.open(QIODevice::WriteOnly))
		{
			emit error(tr("Failed to save file: ") + path, true);
			return PARSE_FILE_CRITICAL_ERROR;
		}

		int writeLen = f.write(data);
		f.close();

		if (writeLen != data.length())
		{
			emit error(tr("Failed to save file: ") + path, true);
			return PARSE_FILE_CRITICAL_ERROR;
		}
	}

	return PARSE_FILE_OK;
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
		currentGlobalSite = 0;
		customSiteUsed = false;
		checkedLinks.clear();
		directLinks.clear();
		siteLinks.clear();

		// Get the file extension and compare it against "zip"
		// If the file extension is "zip" already set only "zip"
		// as "binary" files for Http class.
		// If the file extension is not "zip" set "zip" and this extension
		// as "binary" files for Http class.

		QFileInfo fi(wad);
		QString extension = fi.suffix();
		QStringList expectedExtensions;
		expectedExtensions << extension;
		if (extension.compare("zip", Qt::CaseInsensitive) != 0)
		{
			expectedExtensions << "zip";
		}
		http.setBinaryFilesExtensions(expectedExtensions);

		seekedWad = wad;
		nextSite();
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
		QString err = tr("Target directory: \"") + targetDirectory + tr("\" doesn't exist! Aborting");
		emit error(err, true);
		return;
	}

	if (!fi.isWritable())
	{
		QString err = tr("You cannot write to directory: \"") + targetDirectory + tr("\"! Aborting");
		emit error(err, true);
		return;
	}

	seekNextWad();
}

void Wadseeker::size(unsigned int s)
{
	emit wadSize(s);
}

void Wadseeker::sizeUpdate(unsigned howMuch, unsigned howMuchSum, unsigned percent)
{
	emit wadCurrentDownloadedSize(howMuchSum, percent);
}
