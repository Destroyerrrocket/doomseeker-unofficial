//------------------------------------------------------------------------------
// serverconsole.cpp
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
#include "serverconsole.h"

ServerConsole::ServerConsole(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	setupUi(this);

	consoleInput = new MemoryLineEdit();
	layout()->addWidget(consoleInput);

	connect(consoleInput, SIGNAL(returnPressed()), this, SLOT(forwardMessage()));
}

void ServerConsole::appendMessage(const QString &message)
{
	QString appendMessage = message;
	if(appendMessage.endsWith('\n')) // Remove the trailing new line since appendPlainText seems to add one automatically.
		appendMessage.chop(1);

	consoleOutput->appendPlainText(appendMessage);
}

void ServerConsole::forwardMessage()
{
	QString msg = consoleInput->text();
	if(msg[0] == ':')
		msg.replace(0, 1, "say ");

	emit messageSent(msg);
	consoleInput->setText("");
}

void ServerConsole::setFocus()
{
	consoleInput->setFocus();
}
