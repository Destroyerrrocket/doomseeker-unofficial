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

#include <QMessageBox>

#include "remoteconsole.h"
#include "passwordDlg.h"
#include "widgets/serverconsole.h"

RemoteConsole::RemoteConsole(Server *server, QWidget *parent) : QMainWindow(parent), protocol(server->rcon())
{
	setupUi(this);
	serverConsole = new ServerConsole();
	console->layout()->addWidget(serverConsole);

	// delete ourself on close
	setAttribute(Qt::WA_DeleteOnClose);

	setWindowIcon(server->icon());
	setWindowTitle(windowTitle() + " - " + server->name());

	if(protocol == NULL)
		return;

	// If we connected show the window
	show();

	connect(actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectFromServer()));
	connect(serverConsole, SIGNAL(messageSent(const QString &)), protocol, SLOT(sendCommand(const QString &)));
	connect(protocol, SIGNAL(disconnected()), this, SLOT(close()));
	connect(protocol, SIGNAL(messageReceived(const QString &)), serverConsole, SLOT(appendMessage(const QString &)));
	connect(protocol, SIGNAL(invalidPassword()), this, SLOT(invalidPassword()));
	connect(protocol, SIGNAL(playerListUpdated()), this, SLOT(updatePlayerList()));

	showPasswordDialog();
}

void RemoteConsole::closeEvent(QCloseEvent *event)
{
	if(protocol->isConnected())
		disconnectFromServer();
	event->accept();
}

void RemoteConsole::invalidPassword()
{
	QMessageBox::critical(NULL, tr("Invalid Password"), tr("The password you entered appears to be invalid."));
	showPasswordDialog();
}

void RemoteConsole::disconnectFromServer()
{
	if(protocol->isConnected())
		protocol->disconnectFromServer();
}

void RemoteConsole::showPasswordDialog()
{
	// Prompt for password.
	PasswordDlg *dlg = new PasswordDlg(this, true);
	connect(dlg, SIGNAL(rejected()), this, SLOT(close()));
	int ret = dlg->exec();
	if(ret == QDialog::Accepted)
	{
		protocol->sendPassword(dlg->connectPassword());
	}
	delete dlg;
}

void RemoteConsole::updatePlayerList()
{
	const QList<Player> &list = protocol->playerList();

	playerTable->setRowCount(list.size());
	for(int i = 0; i < list.size(); ++i)
	{
		QString name = list[i].nameFormatted();
		QTableWidgetItem* newItem = new QTableWidgetItem(name);
		playerTable->setItem(i, 0, newItem);
	}
}
