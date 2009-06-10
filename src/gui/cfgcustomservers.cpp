//------------------------------------------------------------------------------
// cfgcustomservers.cpp
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
#include "gui/cfgcustomservers.h"

CustomServersConfigBox::CustomServersConfigBox(Config *cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	prepareTable();
}

ConfigurationBoxInfo* CustomServersConfigBox::createStructure(Config *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->confBox = new CustomServersConfigBox(cfg, parent);
	ec->boxName = tr("Custom Servers");
	return ec;
}

void CustomServersConfigBox::prepareTable()
{
	model = new QStandardItemModel(this);

	QStringList labels;
	labels << "Engine" << "Host" << "Port";
	model->setHorizontalHeaderLabels(labels);

	QList<QStandardItem* > record;

	record.append(new QStandardItem("A"));
	record.append(new QStandardItem("B"));
	record.append(new QStandardItem("C"));

	model->appendRow(record);

	tvServers->setModel(model);
}

void CustomServersConfigBox::readSettings()
{
	if (model->rowCount() != 0)
	{
		tvServers->resizeColumnsToContents();
	}
	else
	{
		tvServers->setColumnWidth(0, 20);
		tvServers->setColumnWidth(1, 200);
		tvServers->setColumnWidth(2, 100);
	}
}

void CustomServersConfigBox::saveSettings()
{

}
