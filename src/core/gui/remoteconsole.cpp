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
#include <QScopedPointer>
#include <QString>

#include "plugins/engineplugin.h"
#include "serverapi/rconprotocol.h"
#include "serverapi/server.h"
#include "widgets/serverconsole.h"
#include "remoteconsole.h"
#include "rconpassworddialog.h"
#include "strings.h"
#include "ui_remoteconsole.h"

class RemoteConsole::PrivData : public Ui::RemoteConsole
{
public:
	RConProtocol *protocol;
	ServerConsole *serverConsole;
	ServerPtr server;
};

RemoteConsole::RemoteConsole(QWidget *parent) : QMainWindow(parent)
{
	d = new PrivData;
	d->protocol = NULL;

	// Prompt for connection info & password.
	RconPasswordDialog *dlg = new RconPasswordDialog(this, true);
	connect(dlg, SIGNAL(rejected()), this, SLOT(close()));
	while(d->protocol == NULL)
	{
		int ret = dlg->exec();
		if(ret == QDialog::Accepted)
		{
			QString address;
			unsigned short port;
			Strings::translateServerAddress(dlg->serverAddress(), address, port, QString("localhost:%1").arg(dlg->selectedEngine()->data()->defaultServerPort));

			d->server = dlg->selectedEngine()->server(QHostAddress(address), port);
			if(!d->server->hasRcon())
			{
				QMessageBox::critical(this, tr("No RCon support"), tr("The source port selected has no RCon support."));
				close();
				return;
			}
			d->protocol = d->server->rcon();

			if(d->protocol != NULL)
			{
				setWindowIcon(d->server->icon());
				standardInit();

				d->protocol->sendPassword(dlg->connectPassword());
			}
		}
		else
			break;
	}
	delete dlg;
}

RemoteConsole::RemoteConsole(ServerPtr server, QWidget *parent)
: QMainWindow(parent)
{
	d = new PrivData;
	d->protocol = server->rcon();
	d->server = server;
	standardInit();

	setWindowIcon(server->icon());
	changeServerName(server->name());

	if (d->protocol != NULL)
	{
		showPasswordDialog();
	}
	else
	{
		QMessageBox::critical(parent, tr("RCon Failure"),
			tr("Failed to connect RCon to server %1:%2").arg(
			server->address().toString()).arg(server->port()));
	}
}

RemoteConsole::~RemoteConsole()
{
	delete d;
}

void RemoteConsole::changeServerName(const QString &name)
{
	setWindowTitle(name + tr(" - Remote Console"));
}

void RemoteConsole::closeEvent(QCloseEvent *event)
{
	if(d->protocol && d->protocol->isConnected())
		d->protocol->disconnectFromServer();
	event->accept();
}

void RemoteConsole::invalidPassword()
{
	QMessageBox::critical(this, tr("Invalid Password"), tr("The password you entered appears to be invalid."));
	showPasswordDialog();
}

bool RemoteConsole::isValid() const
{
	return d->protocol != NULL;
}

void RemoteConsole::disconnectFromServer()
{
	if (d->protocol)
	{
		d->protocol->disconnectFromServer();
	}
	else
	{
		close();
	}
}

void RemoteConsole::showPasswordDialog()
{
	// Prompt for password.
	RconPasswordDialog *dlg = new RconPasswordDialog(this);
	connect(dlg, SIGNAL(rejected()), this, SLOT(close()));
	int ret = dlg->exec();
	if(ret == QDialog::Accepted)
	{
		d->protocol->sendPassword(dlg->connectPassword());
	}
	delete dlg;

	// Set/Restore focus to the cmd line input.
	d->serverConsole->setFocus();
}

void RemoteConsole::standardInit()
{
	d->setupUi(this);
	d->serverConsole = new ServerConsole();
	d->console->layout()->addWidget(d->serverConsole);

	// delete ourself on close
	setAttribute(Qt::WA_DeleteOnClose);

	if(d->protocol == NULL)
		return;

	// If we connected show the window
	show();

	connect(d->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectFromServer()));
	connect(d->serverConsole, SIGNAL(messageSent(const QString &)), d->protocol, SLOT(sendCommand(const QString &)));
	connect(d->protocol, SIGNAL(disconnected()), this, SLOT(close()));
	connect(d->protocol, SIGNAL(messageReceived(const QString &)), d->serverConsole, SLOT(appendMessage(const QString &)));
	connect(d->protocol, SIGNAL(invalidPassword()), this, SLOT(invalidPassword()));
	connect(d->protocol, SIGNAL(playerListUpdated()), this, SLOT(updatePlayerList()));
	connect(d->protocol, SIGNAL(serverNameChanged(const QString &)), this, SLOT(changeServerName(const QString &)));
}

void RemoteConsole::updatePlayerList()
{
	const QList<Player> &list = d->protocol->players();

	d->playerTable->setRowCount(list.size());
	for(int i = 0; i < list.size(); ++i)
	{
		QString name = list[i].nameFormatted();
		QTableWidgetItem* newItem = new QTableWidgetItem(name);
		d->playerTable->setItem(i, 0, newItem);
	}
}
