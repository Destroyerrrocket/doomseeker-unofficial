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
	QUrl("http://zalewa.dyndns.org/robert/doom/site/index2.php"),
	QUrl("http://supergod.servegame.com/"),
	QUrl("") // empty url is treated here like '\0' in a string
};

Wadseeker::Wadseeker()
{
	currentGlobalSite = 0;

	connect(&http, SIGNAL( error(const QString&) ), this, SLOT( httpError(const QString&) ) );
	connect(&http, SIGNAL( finishedReceiving(QString) ), this, SLOT( finishedReceiving(QString) ) );
	//connect(this, SIGNAL( wadDone(bool, const QString&) ), this, SLOT( seekNextWad(bool, const QString&) ) );
}

Wadseeker::~Wadseeker()
{
}

void Wadseeker::finishedReceiving(QString error)
{
	if (!error.isEmpty())
	{
		qDebug() << "HTTP error: " << error.toAscii().constData();
		return;
	}

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
				strUrl = url.authority();
			}
			else
			{
				strUrl = "";
			}

			QUrl newUrl(strUrl + it->url.toString());
			printf("%s\n", newUrl.toString().toAscii().constData());

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

	nextSite();
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
	qDebug() << "Error:" << errorString;
	nextSite();
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
			checkedLinks.insert(url.toString());
			bGotUrl = true;
		}
	}

	while (!siteLinks.empty() && !bGotUrl)
	{
		url = siteLinks.first();
		siteLinks.removeFirst();
		if (checkedLinks.find(url.toString()) == checkedLinks.end())
		{
			checkedLinks.insert(url.toString());
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
		qDebug() << "no more sites";
		emit wadDone(false, seekedWad);
		seekNextWad();
		return;
	}

	qDebug() << "Next site:" << url.toString();

	http.setSite(url.encodedHost());
	http.sendRequestGet(url.encodedPath());
}

QString Wadseeker::nextWad()
{
	if (currentWad == wadnames.end())
	{
		return QString();
	}

	QString str = *currentWad;
	++currentWad;
	return str;
}

void Wadseeker::seekNextWad()
{
	QString str = nextWad();
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
	currentGlobalSite = 0;
	customSiteUsed = false;
	checkedLinks.clear();
	directLinks.clear();
	siteLinks.clear();

	seekedWad = wad;
	nextSite();
}

void Wadseeker::seekWads(const QStringList& wads)
{
	wadnames = wads;
	currentWad = wadnames.begin();

	seekNextWad();
}
