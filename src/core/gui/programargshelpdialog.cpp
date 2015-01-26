//------------------------------------------------------------------------------
// programargshelpdialog.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "programargshelpdialog.h"

#include "ui_programargshelpdialog.h"
#include "cmdargshelp.h"

class ProgramArgsHelpDialog::PrivData : public Ui::ProgramArgsHelpDialog
{
public:
};


ProgramArgsHelpDialog::ProgramArgsHelpDialog(QWidget *parent)
: QDialog(parent)
{
	d = new PrivData();
	d->setupUi(this);

	QFont font("Courier");
	d->textArea->setFont(font);
	d->textArea->setPlainText(CmdArgsHelp::argsHelp());
}

ProgramArgsHelpDialog::~ProgramArgsHelpDialog()
{
	delete d;
}