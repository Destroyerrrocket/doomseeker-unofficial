//------------------------------------------------------------------------------
// www.cpp
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
#include "protocols/ftp.h"
#include "protocols/http.h"
#include "www.h"

const QString WWW::MESSAGE_IGNORE = tr("%1 is not a HTML file, nor a wanted file, nor a file with \'.zip\' or \'.7z\' extension. Ignoring.");

WWW::WWW()
{
	currentProtocol = NULL;

	ftp = new Ftp();
	http = new Http();

	connect(ftp, SIGNAL( aborted() ), this, SLOT( protocolAborted() ) );
	connect(ftp, SIGNAL( dataReadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(ftp, SIGNAL( done(bool, QByteArray&, int, const QString&) ), this, SLOT( protocolDone(bool, QByteArray&, int, const QString&) ) );
	connect(ftp, SIGNAL( message(const QString&, int) ), this, SLOT( messageSlot(const QString&, int) ) );

	connect(http, SIGNAL( aborted() ), this, SLOT( protocolAborted() ) );
	connect(http, SIGNAL( dataReadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(http, SIGNAL( done(bool, QByteArray&, int, const QString&) ), this, SLOT( protocolDone(bool, QByteArray&, int, const QString&) ) );
	connect(http, SIGNAL( message(const QString&, int) ), this, SLOT( messageSlot(const QString&, int) ) );
	connect(http, SIGNAL( redirect(const QUrl&) ), this, SLOT( get(const QUrl&) ) );
}

WWW::~WWW()
{
	delete ftp;
	delete http;
}

void WWW::abort()
{
	abortExec(true);
}

void WWW::abortExec(bool abortCompletely)
{
	aborting = abortCompletely;
	if (currentProtocol != NULL)
	{
		currentProtocol->abort();
	}
	else
	{
		emit aborted();
	}
}

QUrl WWW::constructValidUrl(const QUrl& url)
{
	QUrl returnUrl = url;
	if (returnUrl.scheme().isEmpty())
	{
		returnUrl.setScheme(processedUrl.scheme());
	}

	if (returnUrl.scheme().isEmpty())
	{
		return QUrl();
	}

	if (returnUrl.host().isEmpty())
	{
		returnUrl.setHost(processedUrl.host());
	}

	if (returnUrl.host().isEmpty())
	{
		return QUrl();
	}

	return returnUrl;
}

void WWW::downloadProgressSlot(int done, int total)
{
	emit downloadProgress(done, total);
}

void WWW::get(const QUrl& url)
{
	QUrl urlValid = constructValidUrl(url);
	if (urlValid.isEmpty())
	{
		emit message(tr("Failed to create valid URL out of \"%1\". Ignoring.\n").arg(url.toString()), Wadseeker::Error);
		emit fail();
		return;
	}

	processedUrl = urlValid;

	if (Http::isHTTPLink(urlValid))
	{
		currentProtocol = http;
		http->get(urlValid);
	}
	else if (Ftp::isFTPLink(urlValid))
	{
		currentProtocol = ftp;
		ftp->get(urlValid);
	}
	else
	{
		currentProtocol = NULL;
		message(tr("Protocol for this site is not supported."), Wadseeker::Error);
		emit fail();
		return;
	}
}

bool WWW::getUrl(const QUrl& url)
{
	if (!isAbsoluteUrl(url))
	{
		return false;
	}

	QUrl urlValid = url;
	if (urlValid.path().isEmpty())
	{
		urlValid.setPath("/");
	}

	if (Http::isHTTPLink(urlValid))
	{
		currentProtocol = http;
		http->get(urlValid);
	}
	else if (Ftp::isFTPLink(urlValid))
	{
		currentProtocol = ftp;
		ftp->get(urlValid);
	}
	else
	{
		currentProtocol = NULL;
		message(tr("Protocol for this site is not supported."), Wadseeker::Error);
		return false;
	}

	return true;
}

bool WWW::isAbsoluteUrl(const QUrl& url)
{
	if (url.scheme().isEmpty() || url.host().isEmpty())
	{
		return false;
	}

	return true;
}

void WWW::messageSlot(const QString& msg, int type)
{
	emit message(msg, type);
}

void WWW::protocolAborted()
{
	currentProtocol = NULL;
	if (!aborting)
	{
		emit fail();
	}
	else
	{
		emit aborted();
	}
}

void WWW::protocolDone(bool success, QByteArray& data, int fileType, const QString& filename)
{
	currentProtocol = NULL;
	if (success)
	{
		emit fileDone(data, filename);
	}
	else
	{
		emit fail();
	}
}

void WWW::setTimeConnectTimeout(int seconds)
{
	Protocol::setTimeConnectTimeoutSeconds(seconds);
}

void WWW::setTimeDownloadTimeout(int seconds)
{
	Protocol::setTimeDownloadTimeoutSeconds(seconds);
}

void WWW::setUserAgent(const QString& agent)
{
	http->setUserAgent(agent);
	setUserAgentEx(agent);
}
