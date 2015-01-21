//------------------------------------------------------------------------------
// wadarchiveclient.h
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id6c75c254_acf8_441c_980d_c044fd80f264
#define id6c75c254_acf8_441c_980d_c044fd80f264

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariant>
#include "wadseekermessagetype.h"

class WadDownloadInfo;

class WadArchiveClient : public QObject
{
Q_OBJECT

public:
	WadArchiveClient();
	~WadArchiveClient();

	void abort();
	void enqueue(const WadDownloadInfo &wad);
	bool isWorking() const;
	void reportBadUrlIfOriginatingFromHere(const QUrl &url);
	void setUserAgent(const QString &userAgent);
	void start();

signals:
	void finished();
	void message(const QString &msg, WadseekerLib::MessageType type);
	void urlFound(const QString &wadName, const QUrl &url);

private:
	class PrivData;
	PrivData *d;

	void emitFinished();
	void parseWadArchiveStructure(const QVariantMap &map);
	void startNextInQueue();

private slots:
	void onQueryFinished();
};

#endif
