//------------------------------------------------------------------------------
// remoteconsole.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __REMOTECONSOLE_H__
#define __REMOTECONSOLE_H__

#include "serverapi/serverptr.h"
#include "dptr.h"

#include <QMainWindow>

class RemoteConsole : public QMainWindow
{
	Q_OBJECT

	public:
		RemoteConsole(QWidget *parent=NULL);
		RemoteConsole(ServerPtr server, QWidget *parent=NULL);
		~RemoteConsole();

		/**
		 * Returns true if the remote console has been successfully created.
		 * Should not be false unless the first constructor is used.
		 */
		bool isValid() const;

	public slots:
		void disconnectFromServer();

	protected:
		void closeEvent(QCloseEvent *event);
		void showPasswordDialog();
		void standardInit();

	protected slots:
		void changeServerName(const QString &name);
		void invalidPassword();
		void updatePlayerList();

	private:
		DPtr<RemoteConsole> d;
};

#endif /* __REMOTECONSOLE_HPP__ */
