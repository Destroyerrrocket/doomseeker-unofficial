//------------------------------------------------------------------------------
// ftp.cpp
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
#include "ftp.h"

Ftp::Ftp()
{
	qFtp = NULL;
}

void Ftp::abortEx()
{
	if (qFtp != NULL)
	{
		qFtp->abort();
		qFtp->close();
		disconnectQFtp();
	}

	emit aborted();
}

void Ftp::commandFinished(int id, bool error)
{
	if (id == listCommandId)
		qFtp->get(queryUrl.path());
}

void Ftp::disconnectQFtp()
{
	if (qFtp != NULL)
	{
		qFtp->disconnect(this, SLOT( dataTransferProgressSlot(qint64, qint64) ) );
		qFtp->disconnect(this, SLOT( doneSlot(bool) ) );
		qFtp->disconnect(this, SLOT( stateChanged(int) ) );
		qFtp = NULL;
	}
}

void Ftp::dataTransferProgressSlot(qint64 done, qint64 total)
{
	dataReadProgressSlot(static_cast<int>(done), size);
}

void Ftp::doneEx(bool error)
{
	if (error)
	{
		emit message(tr("FTP error: %1").arg(qFtp->errorString()), Wadseeker::Error);
		noData = true;
	}

	if (aborting)
	{
		return;
	}

	if (noData)
	{
		QByteArray data = QByteArray();
		disconnectQFtp();
		emit done(false, data, 0, processedFileName);
	}
	else
	{
		QByteArray data = qFtp->readAll();
		disconnectQFtp();
		emit done(true, data, Other, processedFileName);
	}
}

void Ftp::getEx(const QUrl& url)
{
	disconnectQFtp();
	listCommandId = 9999;
	size = 0;

	qFtp = new QFtp(this);

	connect(qFtp, SIGNAL( commandFinished(int, bool) ), this, SLOT ( commandFinished(int, bool) ) );
	connect(qFtp, SIGNAL( dataTransferProgress(qint64, qint64) ), this, SLOT ( dataTransferProgressSlot(qint64, qint64) ) );
	connect(qFtp, SIGNAL( done(bool) ), this, SLOT ( doneSlot(bool) ) );
	connect(qFtp, SIGNAL( listInfo(const QUrlInfo&) ), this, SLOT ( listInfo(const QUrlInfo&) ) );
	connect(qFtp, SIGNAL( stateChanged(int) ), this, SLOT ( stateChanged(int) ) );

	queryUrl = url;
	qFtp->connectToHost(url.host(), url.port(21));
}

bool Ftp::isFTPLink(const QUrl& url)
{
	const QString& scheme = url.scheme();
	if(scheme.compare("ftp", Qt::CaseInsensitive) == 0)
		return true;

	return false;
}

void Ftp::listInfo(const QUrlInfo& i)
{
	if (i.name().compare(queryUrl.path()) == 0)
	{
		emit message(tr("File size: %1 B").arg(i.size()), Wadseeker::Notice);
		size = i.size();
	}
}

void Ftp::stateChanged(int state)
{
	if (state == QFtp::Connected)
	{
		QString login = queryUrl.userName();
		QString password = queryUrl.password();
		qFtp->login(login, password);
	}
	else if (state == QFtp::LoggedIn)
	{
		// Try to determine the size first.
		listCommandId = qFtp->list(queryUrl.path());
	}
}
