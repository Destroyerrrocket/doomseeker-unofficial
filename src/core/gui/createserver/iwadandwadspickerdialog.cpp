//------------------------------------------------------------------------------
// iwadandwadspickerdialog.cpp
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
#include "iwadandwadspickerdialog.h"

#include "ui_iwadandwadspickerdialog.h"

class IwadAndWadsPickerDialog::PrivData : public Ui::IwadAndWadsPickerDialog
{
public:
};


IwadAndWadsPickerDialog::IwadAndWadsPickerDialog(QWidget *parent)
: QDialog(parent)
{
	d = new PrivData();
	d->setupUi(this);
}

IwadAndWadsPickerDialog::~IwadAndWadsPickerDialog()
{
	delete d;
}

QStringList IwadAndWadsPickerDialog::filePaths() const
{
	return d->wadsPicker->filePaths();
}

QString IwadAndWadsPickerDialog::iwadPath() const
{
	return d->iwadPicker->currentIwad();
}

void IwadAndWadsPickerDialog::setIwadByName(const QString &iwad)
{
	d->iwadPicker->setIwadByName(iwad);
}
