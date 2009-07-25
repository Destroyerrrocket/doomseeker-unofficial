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

#include "remoteconsole.h"
#include "passwordDlg.h"

RemoteConsole::RemoteConsole(Server *server, QWidget *parent) : QMainWindow(parent), protocol(server->rcon())
{
	setupUi(this);

	setWindowIcon(server->icon());
	setWindowTitle(windowTitle() + " - " + server->name());

	if(protocol == NULL)
		return;

	// If we connected show the window
	show();

	connect(actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectFromServer()));
	connect(consoleInput, SIGNAL(returnPressed()), this, SLOT(sendCommand()));
	connect(protocol, SIGNAL(disconnected()), this, SLOT(close()));
	connect(protocol, SIGNAL(messageReceived(const QString &)), this, SLOT(receiveMessage(const QString &)));
	connect(protocol, SIGNAL(playerListUpdated()), this, SLOT(updatePlayerList()));

	// Prompt for password.
	PasswordDlg *dlg = new PasswordDlg(this, true);
	connect(dlg, SIGNAL(rejected()), this, SLOT(close()));
	int ret = dlg->exec();
	if(ret == QDialog::Accepted)
	{
		protocol->sendPassword(dlg->connectPassword());
	}
	delete dlg;
	if(ret != QDialog::Accepted)
		return;
}
 
void RemoteConsole::closeEvent(QCloseEvent *event)
{
	if(protocol->isConnected())
		disconnectFromServer();
	event->accept();
}

void RemoteConsole::disconnectFromServer()
{
	if(protocol->isConnected())
		protocol->disconnectFromServer();
}

void RemoteConsole::receiveMessage(const QString &message)
{
	console->appendPlainText(message.trimmed());
}

void RemoteConsole::sendCommand()
{
	protocol->sendCommand(consoleInput->text());
	consoleInput->setText("");
}

void RemoteConsole::updatePlayerList()
{
	const QList<Player> &list = protocol->playerList();

	playerTable->setRowCount(list.size());
	for(unsigned int i = 0;i < list.size();i++)
		playerTable->setItem(i, 0, new QTableWidgetItem(list[i].nameFormatted()));
}
