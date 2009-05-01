//------------------------------------------------------------------------------
// wadseekerinterface.cpp
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
#include "gui/wadseekerinterface.h"
#include "wadseeker/wadseeker.h"

WadSeekerInterface::WadSeekerInterface(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT ( btnClicked(QAbstractButton *) ));
}

void WadSeekerInterface::accept()
{
	Wadseeker ws;
	QStringList list;
	list << "a";
	ws.seekWads(list);
	this->close();
}

void WadSeekerInterface::btnClicked(QAbstractButton *button)
{
	// Figure out what button we pressed and perform its action.
	switch(buttonBox->standardButton(button))
	{
		default:
			break;

		case QDialogButtonBox::Ok:
			this->accept();
			break;

		case QDialogButtonBox::Cancel:
			this->reject();
			break;
	}
}
