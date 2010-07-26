//------------------------------------------------------------------------------
// ircdock.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ircdock.h"

#include <QToolBar>

IRCDock::IRCDock(QWidget* parent)
: QDockWidget(parent)
{
	setupUi(this);
	setupToolbar();

	connect(btnSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ));
	connect(leCommandLine, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ));
}

void IRCDock::sendMessage()
{
	QString message = leCommandLine->text();
	leCommandLine->setText("");
}

void IRCDock::setupToolbar()
{
	QToolBar* pToolBar = new QToolBar(this);
	pToolBar->setMovable(false);

	toolBarConnect = new QAction(QIcon(":/icons/network-connect-3.png"), tr("Connect"), pToolBar);
	toolBarDisconnect = new QAction(QIcon(":/icons/network-disconnect-3.png"), tr("Disconnect"), pToolBar);

	pToolBar->addAction(toolBarConnect);
	pToolBar->addAction(toolBarDisconnect);

	verticalLayout->insertWidget(0, pToolBar);
	connect(pToolBar, SIGNAL( actionTriggered(QAction*) ), this, SLOT( toolBarAction(QAction*) ) );
}

void IRCDock::toolBarAction(QAction* pAction)
{
	if (pAction == toolBarConnect)
	{

	}
}
