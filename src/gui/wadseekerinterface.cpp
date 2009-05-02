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
#include "main.h"

WadSeekerInterface::WadSeekerInterface(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
	connect(&wadseeker, SIGNAL( allDone() ), this, SLOT( allDone() ) );
	connect(&wadseeker, SIGNAL( wadDone(bool, const QString&) ), this, SLOT( wadDone(bool, const QString&) ) );
}

void WadSeekerInterface::accept()
{
	SettingsData* setting;
	setting = Main::config->setting("WadseekerTargetDirectory");

	buttonBox->setEnabled(false);

	QStringList list;
	//list << leWadName->text();
	list << "test.wad";
	wadseeker.setTargetDirectory(setting->string());
	wadseeker.seekWads(list);
}

void WadSeekerInterface::allDone()
{
	qDebug() << "All done!";
	buttonBox->setEnabled(true);
}

void WadSeekerInterface::wadDone(bool bFound, const QString& wadname)
{
	qDebug() << "Wad" << wadname << "done! Found:" << bFound;
}
