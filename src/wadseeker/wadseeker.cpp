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

#define GLOBAL_SITE_LINKS 1

QUrl Wadseeker::globalSiteLinks[] =
{
	QUrl("http://zalewa.dyndns.org/robert/doom/site/index2.php")
};

Wadseeker::Wadseeker()
{
	currentGlobalSite = 0;

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
		wantedFileNames << fi.fileName();

	QList<Link> list = http.links();
	QList<Link>::iterator it;

	for (it = list.begin(); it != list.end(); ++it)
	{
		if (isDirectLinkToFile(wantedFileNames, it->url))
		{
			QString strUrl;
			if (it->url.authority().isEmpty())
			{
				strUrl = url.authority();
			}
			else
			{
				strUrl = it->url.authority();
			}

			QUrl(strUrl + it->url.toString());
			directLinks.append(*it);
		}
		else
		{
			// here we append all links that contain this filename somewhere else than in path.
		}
	}

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
	Link link;
	bool bGotUrl = false;
	while (!directLinks.empty() && !bGotUrl)
	{
		link = directLinks.first();
		directLinks.removeFirst();
		if (checkedLinks.find(link.url.toString()) == checkedLinks.end())
		{
			url = link.url;
			bGotUrl = true;
		}
	}

	while (!siteLinks.empty() && !bGotUrl)
	{
		link = siteLinks.first();
		siteLinks.removeFirst();
		if (checkedLinks.find(link.url.toString()) == checkedLinks.end())
		{
			url = link.url;
			bGotUrl = true;
		}
	}

	if (currentGlobalSite < GLOBAL_SITE_LINKS && !bGotUrl)
	{
		url = globalSiteLinks[currentGlobalSite];
		++currentGlobalSite;
		bGotUrl = true;
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
