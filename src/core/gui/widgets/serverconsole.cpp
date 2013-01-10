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
#include "configuration/doomseekerconfig.h"
#include "serverconsole.h"
#include "strings.h"

#include <QRegExp>

ServerConsole::ServerConsole(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	setupUi(this);

	consoleOutput->setHtml("");

	consoleInput = new MemoryLineEdit();
	layout()->addWidget(consoleInput);

	connect(consoleInput, SIGNAL(returnPressed()), this, SLOT(forwardMessage()));
}

void ServerConsole::appendMessage(const QString &message)
{
	QString appendMessage = Qt::escape(message);
	if(appendMessage.endsWith('\n')) // Remove the trailing new line since appendPlainText seems to add one automatically.
		appendMessage.chop(1);

	// Process colors
	if(gConfig.doomseeker.bColorizeServerConsole)
		appendMessage = Strings::colorizeString(appendMessage);
	else
	{
		static const QRegExp colorCode("\034(\\[[a-zA-Z0-9]*\\]|[a-v+\\-!*])");
		appendMessage.remove(colorCode);
	}

	// Emulate append since we need to force HTML on (append auto detects which fails if &lt; or < is not found).
	consoleOutput->moveCursor(QTextCursor::End);
	consoleOutput->insertHtml("<br>" + appendMessage);
	consoleOutput->moveCursor(QTextCursor::End);
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
