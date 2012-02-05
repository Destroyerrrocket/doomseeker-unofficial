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

#include <QObject>

class CommandLineInfo;
class Server;
class QUrl;
class QWidget;

class ConnectionHandler : public QObject
{
	Q_OBJECT

	public:
		ConnectionHandler(Server *server, QWidget *parent=NULL, bool handleResponse=false);

		void	run();

		static bool checkWadseekerValidity(QWidget *parent=NULL);
		static ConnectionHandler *connectByUrl(const QUrl &url);
		/**
		 *	Generates command line info for specified server.
		 *
		 *	@param server - command line will be generated for this server
		 *	@param [out] cli - generated command line
		 *	@param errorCaption - caption used for QMessageBox in case of an
		 *		error
		 *	@param hadMissing - If set to a non-NULL value the pointer will be
		 *		set to true if Wadseeker downloaded files.
		 *	@return true on success, false otherwise.
		 */
		static bool obtainJoinCommandLine(QWidget *parent, const Server* server, CommandLineInfo& cli, const QString& errorCaption, bool *hadMissing=NULL);

	protected:
		void	refreshToJoin();

	protected slots:
		void	checkResponse(Server *server, int response);

	signals:
		void	finished(int response);

	private:
		bool	handleResponse;
		Server	*server;
		QWidget	*parent;
};

#endif
