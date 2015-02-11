//------------------------------------------------------------------------------
// connectionhandler.h
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
// Copyright (C) 2012 Braden Obrzut <admin@maniacsvault.net>
//                    "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#ifndef __CONNECTIONHANDLER_H__
#define __CONNECTIONHANDLER_H__

#include "serverapi/serverptr.h"
#include "dptr.h"
#include <QObject>

class CommandLineInfo;
class JoinError;
class Server;
class QUrl;
class QWidget;

/**
 * This is needed so we can register the ConnectionHandler with
 * QDesktopServices.
 */
class PluginUrlHandler : public QObject
{
	Q_OBJECT

	public:
		static void registerAll();
		static void registerScheme(const QString &scheme);

	public slots:
		void handleUrl(const QUrl &url);

	private:
		static PluginUrlHandler *instance;
};

class ConnectionHandler : public QObject
{
	Q_OBJECT

	public:
		ConnectionHandler(ServerPtr server, QWidget *parentWidget=NULL, bool handleResponse=false);
		~ConnectionHandler();

		void run();

		static ConnectionHandler *connectByUrl(const QUrl &url);

	protected:
		void finish(int response);
		void refreshToJoin();

	protected slots:
		void checkResponse(const ServerPtr &server, int response);

	signals:
		void finished(int response);

	private:
		DPtr<ConnectionHandler> d;

		void buildJoinCommandLine();
		void runCommandLine(const CommandLineInfo &cli);

	private slots:
		void onCommandLineBuildFinished();
};

#endif
