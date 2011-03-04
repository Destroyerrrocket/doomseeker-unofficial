//------------------------------------------------------------------------------
// logdock.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "logdock.h"
#include "log.h"
#include <QClipboard>

LogDock::LogDock(QWidget* parent) : QDockWidget(parent)
{
	setupUi(this);
	this->toggleViewAction()->setIcon(QIcon(":/icons/log.png"));

	dockWidgetContents->setHintSize(175, 200);

	connect(btnClear, SIGNAL( clicked() ), this, SLOT( clearContent() ) );
	connect(btnCopy, SIGNAL( clicked() ), this, SLOT( btnCopyClicked() ) );
}

void LogDock::appendLogEntry(const QString& entry)
{
	teContent->moveCursor(QTextCursor::End);
	teContent->insertPlainText(entry);
}

void LogDock::clearContent()
{
	gLog.clearContent();
	teContent->document()->clear();
}

void LogDock::btnCopyClicked()
{
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(teContent->document()->toPlainText());
}
