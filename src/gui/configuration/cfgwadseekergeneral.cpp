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
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QMessageBox>

CFGWadseekerGeneral::CFGWadseekerGeneral(IniSection& cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	cbTargetDirectory->setCompleter(new QCompleter(new QDirModel()));
}

void CFGWadseekerGeneral::fillTargetDirectoryComboBox()
{
	cbTargetDirectory->clear();
	cbTargetDirectory->addItems(Main::config["WadPaths"]->split(";", QString::SkipEmptyParts));
}

void CFGWadseekerGeneral::readSettings()
{
	fillTargetDirectoryComboBox();

	cbTargetDirectory->setEditText(config["TargetDirectory"]);
	spinConnectTimeout->setValue(config["ConnectTimeoutSeconds"]);
	spinDownloadTimeout->setValue(config["DownloadTimeoutSeconds"]);
}

void CFGWadseekerGeneral::saveSettings()
{
	config["TargetDirectory"] = cbTargetDirectory->currentText();
	QFileInfo targetDirectoryInfo(cbTargetDirectory->currentText());
	if(!targetDirectoryInfo.isWritable())
	{
		QMessageBox::warning(this, tr("Unwritable Target"), tr("The target directory you selected for Wadseeker can not be written to."));
	}
	// Also take a look at the file paths configuration.  Warn if it is not on the list.
	bool pathPossible = false;
	foreach(QString possiblePath, Main::config["WadPaths"]->split(";", QString::SkipEmptyParts))
	{
		if(possiblePath == cbTargetDirectory->currentText())
			pathPossible = true;
	}
	if(!pathPossible)
		QMessageBox::warning(this, tr("Target not on List"), tr("The specified target directory could not be found on the file paths list."));

	config["ConnectTimeoutSeconds"] = spinConnectTimeout->value();
	config["DownloadTimeoutSeconds"] = spinDownloadTimeout->value();
}
