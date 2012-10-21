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

	// Settings defined in this widget are ATM unused.
	widgetTimeouts->setVisible(false);

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
	spinMaxConcurrentSiteSeeks->setValue(gConfig.wadseeker.maxConcurrentSiteDownloads);
	spinMaxConcurrentWadDownloads->setValue(gConfig.wadseeker.maxConcurrentWadDownloads);
}

void CFGWadseekerGeneral::saveSettings()
{
	gConfig.wadseeker.targetDirectory = cbTargetDirectory->currentText();

	QFileInfo targetDirectoryInfo(cbTargetDirectory->currentText());
	if(!targetDirectoryInfo.isWritable())
	{
		QMessageBox::warning(this, tr("Wadseeker - error"),
			tr("The target directory you selected for Wadseeker can not be written to."));
	}
	else
	{
		// If path seems valid:
		// Also take a look at the file paths configuration.  Warn if it is not on the list.
		bool pathPossible = false;

		QFileInfo wadseekerTargetDirectoryFileInfo(cbTargetDirectory->currentText());
		foreach(QString possiblePath, gConfig.doomseeker.wadPaths)
		{
			// Bring paths to QFileInfo before string comparison. Two same paths
			// may have different string representations.
			QFileInfo possiblePathFileInfo(possiblePath);

			if (possiblePathFileInfo == wadseekerTargetDirectoryFileInfo)
			{
				pathPossible = true;
				break;
			}
		}

		if (!pathPossible)
		{
			QMessageBox::warning(this, tr("Wadseeker - error"),
				tr("The specified target directory for Wadseeker could not be found on the file (WAD) paths list.\n\n"
					"Doomseeker will automatically add this path to the file search paths."));
		}
	}

	gConfig.wadseeker.connectTimeoutSeconds = spinConnectTimeout->value();
	gConfig.wadseeker.downloadTimeoutSeconds = spinDownloadTimeout->value();
	gConfig.wadseeker.maxConcurrentSiteDownloads = spinMaxConcurrentSiteSeeks->value();
	gConfig.wadseeker.maxConcurrentWadDownloads = spinMaxConcurrentWadDownloads->value();
}
