//------------------------------------------------------------------------------
// ip2cupdatebox.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ip2cupdatebox.h"
#include "ui_ip2cupdatebox.h"

#include "doomseekerfilepaths.h"

#include <QDateTime>
#include "ip2c/ip2cupdater.h"

DClass<IP2CUpdateBox> : public Ui::IP2CUpdateBox
{
public:
	IP2CUpdater *ip2cUpdater;
};

DPointered(IP2CUpdateBox)

IP2CUpdateBox::IP2CUpdateBox(QWidget* parent)
: QDialog(parent)
{
	d->setupUi(this);

	connect(d->btnUpdate, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect(d->btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	d->progressBar->hide();

	d->ip2cUpdater = new IP2CUpdater(this);
	this->connect(d->ip2cUpdater, SIGNAL(updateNeeded(int)), SLOT(updateInfo(int)));
	d->ip2cUpdater->needsUpdate(DoomseekerFilePaths::ip2cDatabase());

	start();
}

IP2CUpdateBox::~IP2CUpdateBox()
{
}

void IP2CUpdateBox::start()
{
	QString filePath = DoomseekerFilePaths::ip2cDatabase();

	d->lblIP2CFileLocation->setText(filePath);

	QFileInfo fileInfo(filePath);
	if (fileInfo.exists())
	{
		d->lblDatabaseStatus->setText(tr("Verifying checksum ..."));
		d->progressBar->show();
	}
	else
	{
		d->lblDatabaseStatus->setText(tr("IP2C database file was not found. "
				"Precompiled database will be used. "
				"Use update button if you want to download the newest database."));
	}
}

void IP2CUpdateBox::updateInfo(int status)
{
	d->progressBar->hide();
	switch (status)
	{
	case IP2CUpdater::UpdateNeeded:
		d->lblDatabaseStatus->setText(tr("Update required."));
		break;
	case IP2CUpdater::UpToDate:
		d->lblDatabaseStatus->setText(tr("Database is up-to-date."));
		break;
	case IP2CUpdater::UpdateCheckError:
		d->lblDatabaseStatus->setText(tr("Database status check failed. See log for details."));
		break;
	default:
		d->lblDatabaseStatus->setText(tr("Unhandled update check status."));
		break;
	}
}
