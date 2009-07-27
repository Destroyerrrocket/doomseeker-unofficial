//------------------------------------------------------------------------------
// remoteconsole.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __REMOTECONSOLE_H__
#define __REMOTECONSOLE_H__

#include "ui_remoteconsole.h"
#include "server.h"

class RemoteConsole : public QMainWindow, private Ui::RemoteConsole
{
	Q_OBJECT

	public:
		RemoteConsole(Server *server, QWidget *parent=NULL);

	public slots:
		void	disconnectFromServer();

	protected:
		void	closeEvent(QCloseEvent *event);

	protected slots:
		void	receiveMessage(const QString &message);
		void	sendCommand();
		void	updatePlayerList();

	private:
		RConProtocol		*protocol;
};

#endif /* __REMOTECONSOLE_HPP__ */