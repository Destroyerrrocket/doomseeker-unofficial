//------------------------------------------------------------------------------
// iwadandwadspickerdialog.h
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
#ifndef idc56cfbd4_25dc_4818_96d3_71452acc62e8
#define idc56cfbd4_25dc_4818_96d3_71452acc62e8

#include "global.h"
#include <QDialog>

class MAIN_EXPORT IwadAndWadsPickerDialog : public QDialog
{
public:
	IwadAndWadsPickerDialog(QWidget *parent);
	virtual ~IwadAndWadsPickerDialog();

	QStringList filePaths() const;
	QString iwadPath() const;
	void setIwadByName(const QString &iwad);

private:
	class PrivData;
	PrivData *d;
};

#endif