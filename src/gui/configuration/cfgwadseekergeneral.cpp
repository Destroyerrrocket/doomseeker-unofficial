//------------------------------------------------------------------------------
// cfgwadseekergeneral.cpp
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
#include "main.h"
#include "cfgwadseekergeneral.h"
#include "configuration/doomseekerconfig.h"
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QMessageBox>

CFGWadseekerGeneral::CFGWadseekerGeneral(QWidget* parent) 
: ConfigurationBaseBox(parent)
{
	setupUi(this);

	cbTargetDirectory->setCompleter(new QCompleter(new QDirModel()));
}

void CFGWadseekerGeneral::fillTargetDirectoryComboBox()
{
	cbTargetDirectory->clear();
	cbTargetDirectory->addItems(gConfig.doomseeker.wadPaths);
}

void CFGWadseekerGeneral::readSettings()
{
	fillTargetDirectoryComboBox();

	cbTargetDirectory->setEditText(gConfig.wadseeker.targetDirectory);
	spinConnectTimeout->setValue(gConfig.wadseeker.connectTimeoutSeconds);
	spinDownloadTimeout->setValue(gConfig.wadseeker.downloadTimeoutSeconds);
}

void CFGWadseekerGeneral::saveSettings()
{
	gConfig.wadseeker.targetDirectory = cbTargetDirectory->currentText();
	QFileInfo targetDirectoryInfo(cbTargetDirectory->currentText());
	if(!targetDirectoryInfo.isWritable())
	{
		QMessageBox::warning(this, tr("Unwritable Target"), tr("The target directory you selected for Wadseeker can not be written to."));
	}
	// Also take a look at the file paths configuration.  Warn if it is not on the list.
	bool pathPossible = false;
	foreach(QString possiblePath, gConfig.doomseeker.wadPaths)
	{
		if(possiblePath == cbTargetDirectory->currentText())
		{
			pathPossible = true;
		}
	}
	if(!pathPossible)
	{
		QMessageBox::warning(this, tr("Target not on List"), tr("The specified target directory could not be found on the file paths list."));
	}

	gConfig.wadseeker.connectTimeoutSeconds = spinConnectTimeout->value();
	gConfig.wadseeker.downloadTimeoutSeconds = spinDownloadTimeout->value();
}
