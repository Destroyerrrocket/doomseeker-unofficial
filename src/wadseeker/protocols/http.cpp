//------------------------------------------------------------------------------
// http.cpp
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
#include "http.h"
#include "../html.h"
#include <QTcpSocket>

Http::Http()
{
	qHttp = NULL;

	noData = false;
	redirected = false;
}

void Http::abortEx()
{
	if (qHttp != NULL)
	{
		qHttp->abort();
	}
}

QString Http::attachmentInformation(const QHttpHeader& header, QString& filename)
{
	QString val = header.value("Content-Disposition");
	QStringList values = val.split(";");
	QStringList::iterator it;
	for (it = values.begin(); it != values.end(); ++it)
	{
		if (it->trimmed().compare("attachment") == 0)
		{
			++it;
			if (it == values.end())
				break;

			QString ret = it->trimmed();
			QByteArray asciiStr = ret.toAscii();
			CHtml html(asciiStr);
			filename = html.htmlValue("filename");
			return ret;
		}
	}

	return QString();
}

void Http::disconnectQHttp()
{
	if (qHttp != NULL)
	{
		qHttp->disconnect(this, SLOT( dataReadProgressSlot(int, int) ) );
		qHttp->disconnect(this, SLOT( doneSlot(bool) ) );
		qHttp->disconnect(this, SLOT( headerReceived(const QHttpResponseHeader&) ) );
		qHttp->disconnect(this, SLOT( stateChanged(int) ) );
		qHttp = NULL;
	}
}

void Http::doneEx(bool error)
{
	if (error)
	{
		emit message(tr("HTTP error: %1").arg(qHttp->errorString()), Wadseeker::Error);
		noData = true;
	}

	if (redirected)
	{
		emit redirect(redirectUrl);
		return;
	}

	if (aborting)
	{
		return;
	}

	if (noData)
	{
		QByteArray data = QByteArray();
		disconnectQHttp();
		emit done(false, data, 0, processedFileName);
	}
	else
	{
		QByteArray data = qHttp->readAll();
		disconnectQHttp();
		emit done(true, data, fileType, processedFileName);
	}
}

void Http::getEx(const QUrl& url)
{
	disconnectQHttp();

	qHttp = new QHttp(this);

	connect(qHttp, SIGNAL( dataReadProgress(int, int) ), this, SLOT ( dataReadProgressSlot(int, int) ) );
	connect(qHttp, SIGNAL( done(bool) ), this, SLOT ( doneSlot(bool) ) );
	connect(qHttp, SIGNAL( responseHeaderReceived(const QHttpResponseHeader&) ), this, SLOT ( headerReceived(const QHttpResponseHeader&) ) );
	connect(qHttp, SIGNAL( stateChanged(int) ), this, SLOT ( stateChanged(int) ) );

	noData = false;
	qHttp->setHost(url.host(), url.port(80));
	QString query = url.encodedPath();
	if (!url.encodedQuery().isNull())
	{
		query += "?" + url.encodedQuery();
	}

	if (query.size() == 0 || query[0] != '/')
	{
		query.prepend('/');
	}

	qHttp->get(query);
}

void Http::headerReceived(const QHttpResponseHeader& resp)
{
	QString attachmentInfo;
	QString tmp;
	// qDebug() << resp.toString();

	// Set defaults
	noData = false;
	redirected = false;

	switch (resp.statusCode())
	{
		case OK:
			if (isHTMLFile(resp))
			{
				fileType = Html;
			}
			else
			{
				fileType = Other;
			}
			if (resp.hasContentLength())
			{
				emit message(tr("File size: %1 B").arg(resp.contentLength()), Wadseeker::Notice);
			}

			break;

		case Redirect:
			attachmentInfo = attachmentInformation(resp, processedFileName);

			if (!attachmentInfo.isEmpty())
			{
				fileType = Other;
				emit message(tr("Downloading attached file: %1").arg(processedFileName), Wadseeker::Notice);
			}
			else
			{
				noData = true;
				emit message(tr("Redirecting"), Wadseeker::Notice);
				tmp = resp.value("Location");
				if (tmp.isEmpty())
				{
					emit message(tr("Redirect header was received but no location was specified. Aborting.\n").arg(resp.value("Location")), Wadseeker::Error);
					abort();
				}
				else
				{
					redirected = true;
					redirectUrl = tmp;
				}
			}
			break;

		default:
			noData = true;
			emit message(tr("HTTP response %1 - %2.").arg(QString::number(resp.statusCode())).arg(resp.reasonPhrase()), Wadseeker::Error);
			break;
	}
}

bool Http::isHTMLFile(const QHttpHeader& http)
{
	if (!http.hasKey("Content-type"))
	{
		return false;
	}

	QString contentType = http.value("Content-type");

	if (contentType.contains("text/html", Qt::CaseInsensitive))
	{
		return true;
	}

	return false;
}

bool Http::isHTTPLink(const QUrl& url)
{
	const QString& scheme = url.scheme();
	if(scheme.compare("http", Qt::CaseInsensitive) == 0)
		return true;

	return false;
}

void Http::stateChanged(int state)
{
	if (state == QHttp::Unconnected && aborting)
	{
		disconnectQHttp();
		emit aborted();
	}
}
