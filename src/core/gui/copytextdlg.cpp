//------------------------------------------------------------------------------
// copytextdlg.cpp
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
#include "copytextdlg.h"
#include <QClipboard>

class CopyTextDlg::PrivData : public Ui::CopyTextDlg
{
};

CopyTextDlg::CopyTextDlg(const QString& content, const QString& description, QWidget* parent) : QDialog(parent)
{
	d = new PrivData;
	d->setupUi(this);

	connect(d->btnCopy, SIGNAL( clicked() ), SLOT( copyContent() ) );

	if (!description.isNull())
	{
		d->lblDescription->setText(description);
	}

	d->teContent->document()->setPlainText(content);
}

CopyTextDlg::~CopyTextDlg()
{
	delete d;
}

void CopyTextDlg::copyContent()
{
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(d->teContent->document()->toPlainText());
}
