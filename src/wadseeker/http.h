//------------------------------------------------------------------------------
// http.h
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
#ifndef __HTTP_H_
#define __HTTP_H_

#include <QDebug>
#include <QHttp>
#include <QHttpRequestHeader>
#include <QHttpResponseHeader>
#include <QList>
#include <QStringList>
#include <Qt>
#include <QUrl>
#include <QWaitCondition>

struct Link
{
	QUrl 		url;
	QString 	text;

	bool 		pathEndsWith(const QStringList& ends)
	{
		QString str = url.encodedPath();
		QStringList::const_iterator it;
		for (it = ends.begin(); it != ends.end(); ++it)
		{
			if (str.endsWith(*it, Qt::CaseInsensitive))
			{
				return true;
			}
		}

		return false;
	}

	bool		isHttpLink()
	{
		const QString& scheme = url.scheme();
		if(scheme.isEmpty() || scheme.compare("http", Qt::CaseInsensitive) == 0)
			return true;

		return false;
	}

	/**
	 *	@param comparePage		- if not empty checks if URL refers to the same host as this param
	 *	@return true if URL points to another server
	 */
	bool		isRemote(const QUrl& comparePage)
	{
		QString str1 = url.encodedHost();
		QString str2 = comparePage.encodedHost();

		if (str1.isEmpty())
		{
			return false;
		}

		if (!comparePage.isEmpty())
		{
			if (str1.compare(str2, Qt::CaseInsensitive) != 0)
			{
				return true;
			}
		}
		else
		{
			return true;
		}

		return false;
	}

	/**
	 *	@return true if the URL refers to the same page (for example URLs with '#')
	 */
	bool		isTheSamePage(const QUrl& comparePage)
	{
		QString str1 = url.encodedHost();
		QString str2 = comparePage.encodedHost();

		if (!str1.isEmpty() && str1.compare(str2, Qt::CaseInsensitive) != 0)
		{
			return false;
		}

		str1 = url.encodedQuery();
		str2 = comparePage.encodedQuery();
		QString str3 = url.encodedPath();
		QString str4 = comparePage.encodedPath();

		if (str1.compare(str2, Qt::CaseInsensitive) == 0
			&& str3.compare(str4, Qt::CaseInsensitive) == 0)
		{
			return true;
		}

		return false;
	}

	/**
	 * @return true if URL begins from javascript: phrase
	 */
	bool		isJavascriptURL()
	{
		return url.toString().startsWith("javascript:", Qt::CaseInsensitive);
	}
};

class Http : public QObject
{
	Q_OBJECT

	public:
		enum STATUS_CODES
		{
			STATUS_OK 		= 200,
			STATUS_REDIRECT = 302
		};

		Http();
		Http(QString);
		~Http();

		QByteArray&		lastData() { return data; }
		int				lastResponseCode() const { return responseCode; }
		const QString&	lastResponsePhrase() const { return responsePhrase; }
		QList<Link>		links();
		void 			sendRequestGet(QString);
		void 			setSite(const QString&);

	signals:
		void dataReceived(int statusCode);
		void error(const QString&);
		void finishedReceiving(QString error);

	protected slots:
		void done(bool);
		void headerReceived(const QHttpResponseHeader&);
		void read(const QHttpResponseHeader&);

	protected:
		QByteArray				data;
		bool					dontSendFinishedReceiving;
		QHttp 					qHttp;
		int						responseCode;
		QString					responsePhrase;
		QString 				site;
		QWaitCondition			waitCondition;

		/**
		 * Capitalizes all keywords
		 */
		void					capitalizeTags(QByteArray&);

		/**
		 * Finds a HTML tag starting from index in the byte array.
		 * @param byte		- array that will be searched
		 * @param beginAt 	- index from which searching starts
		 * @param end 		- end index of a tag
		 * @return 			- begin index of a tag
		 */
		int						findTag(QByteArray& byte, int beginAt, int* end);

		/**
		 * You put something like HREF="http://127.0.0.1" and it retrieves the thing after '=' without the "".
		 * @param byte			- array that will be searched
		 * @param beginIndex 	- index from which parsing starts
		 * @param endIndex		- index at which parsing ends
		 * @return 				- trimmed value, without white-spaces and quotes.
		 */
		QString					htmlValue(QByteArray& byte, int beginIndex, int endIndex);

	private:
		void					construct();
};

#endif
