//------------------------------------------------------------------------------
// ip2cupdatebox.cpp
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
#include "ip2cupdatebox.h"
#include "ui_ip2cupdatebox.h"

#include "doomseekerfilepaths.h"

#include <QDateTime>

DClass<IP2CUpdateBox> : public Ui::IP2CUpdateBox
{
};

DPointered(IP2CUpdateBox)

IP2CUpdateBox::IP2CUpdateBox(QWidget* parent)
: QDialog(parent)
{
	d->setupUi(this);

	connect(d->btnUpdate, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect(d->btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

	updateInfo();
}

IP2CUpdateBox::~IP2CUpdateBox()
{
}

void IP2CUpdateBox::updateInfo()
{
	QString filePath = DoomseekerFilePaths::ip2cDatabase();

	d->lblIP2CFileLocation->setText(filePath);

	QFileInfo fileInfo(filePath);
	if (fileInfo.exists())
	{
		QDateTime lastModified = fileInfo.lastModified();
		QDateTime current = QDateTime::currentDateTime();

		int days = lastModified.daysTo(current);

		QString ageString = tr("This database is %n days old.", "", days);
		d->lblDatabaseAge->setText(ageString);
	}
	else
	{
		d->lblDatabaseAge->setText(tr("This file cannot be found. Precompiled database will be used. Use update button if you want to fix this problem."));
	}




}
