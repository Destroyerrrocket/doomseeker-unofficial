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
#include "cfgwadseekergeneral.h"
#include "ui_cfgwadseekergeneral.h"

#include "configuration/doomseekerconfig.h"
#include "pathfinder/filesearchpath.h"
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QMessageBox>

class CFGWadseekerGeneral::PrivData : public Ui::CFGWadseekerGeneral
{
};

CFGWadseekerGeneral::CFGWadseekerGeneral(QWidget* parent)
: ConfigurationBaseBox(parent)
{
	d = new PrivData;
	d->setupUi(this);

	// Settings defined in this widget are ATM unused.
	d->widgetTimeouts->setVisible(false);

	d->cbTargetDirectory->setCompleter(new QCompleter(new QDirModel()));
}

CFGWadseekerGeneral::~CFGWadseekerGeneral()
{
	delete d;
}

void CFGWadseekerGeneral::fillTargetDirectoryComboBox()
{
	d->cbTargetDirectory->clear();
	d->cbTargetDirectory->addItems(gConfig.doomseeker.wadPathsOnly());
}

void CFGWadseekerGeneral::readSettings()
{
	fillTargetDirectoryComboBox();

	d->cbTargetDirectory->setEditText(gConfig.wadseeker.targetDirectory);
	d->spinConnectTimeout->setValue(gConfig.wadseeker.connectTimeoutSeconds);
	d->spinDownloadTimeout->setValue(gConfig.wadseeker.downloadTimeoutSeconds);
	d->spinMaxConcurrentSiteSeeks->setValue(gConfig.wadseeker.maxConcurrentSiteDownloads);
	d->spinMaxConcurrentWadDownloads->setValue(gConfig.wadseeker.maxConcurrentWadDownloads);
}

void CFGWadseekerGeneral::saveSettings()
{
	gConfig.wadseeker.targetDirectory = d->cbTargetDirectory->currentText();

	QFileInfo targetDirectoryInfo(d->cbTargetDirectory->currentText());
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

		QFileInfo wadseekerTargetDirectoryFileInfo(d->cbTargetDirectory->currentText());
		foreach (FileSearchPath possiblePath, gConfig.doomseeker.wadPaths)
		{
			// Bring paths to QFileInfo before string comparison. Two same paths
			// may have different string representations.
			// TODO: Consider recursive paths.
			QFileInfo possiblePathFileInfo(possiblePath.path());

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

	gConfig.wadseeker.connectTimeoutSeconds = d->spinConnectTimeout->value();
	gConfig.wadseeker.downloadTimeoutSeconds = d->spinDownloadTimeout->value();
	gConfig.wadseeker.maxConcurrentSiteDownloads = d->spinMaxConcurrentSiteSeeks->value();
	gConfig.wadseeker.maxConcurrentWadDownloads = d->spinMaxConcurrentWadDownloads->value();
}
