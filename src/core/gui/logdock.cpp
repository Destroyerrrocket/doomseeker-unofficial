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
#include "ui_logdock.h"
#include <QClipboard>

class LogDock::PrivData : public Ui::LogDock
{
};

LogDock::LogDock(QWidget* parent) : QDockWidget(parent)
{
	d = new PrivData;
	d->setupUi(this);
	this->toggleViewAction()->setIcon(QIcon(":/icons/log.png"));

	d->dockWidgetContents->setHintSize(175, 200);

	connect(d->btnClear, SIGNAL( clicked() ), this, SLOT( clearContent() ) );
	connect(d->btnCopy, SIGNAL( clicked() ), this, SLOT( btnCopyClicked() ) );
}

LogDock::~LogDock()
{
	delete d;
}

void LogDock::appendLogEntry(const QString& entry)
{
	d->teContent->moveCursor(QTextCursor::End);
	d->teContent->insertPlainText(entry);
}

void LogDock::clearContent()
{
	gLog.clearContent();
	d->teContent->document()->clear();
}

void LogDock::btnCopyClicked()
{
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(d->teContent->document()->toPlainText());
}
