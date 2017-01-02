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

DClass<CFGWadseekerGeneral> : public Ui::CFGWadseekerGeneral
{
public:
	bool completerActive;

	QString targetDirectory() const
	{
		return cbTargetDirectory->currentText().trimmed();
	}
};

DPointered(CFGWadseekerGeneral)

CFGWadseekerGeneral::CFGWadseekerGeneral(QWidget* parent)
: ConfigPage(parent)
{
	d->setupUi(this);
	d->completerActive = false;

	// Settings defined in this widget are ATM unused.
	d->widgetTimeouts->setVisible(false);
	d->lblDirectoryWarning->setPixmap(QPixmap(":/icons/exclamation_16.png"));
	d->lblDirectoryWarning->hide();
	d->lblDirectoryWarning->setWordWrap(true);

	this->connect(d->cbTargetDirectory, SIGNAL(editTextChanged(QString)),
		SIGNAL(validationRequested()));
}

CFGWadseekerGeneral::~CFGWadseekerGeneral()
{
}

void CFGWadseekerGeneral::activateCompleter()
{
	// Lazy activation of completer prevents floppy drive clicking
	// on QComboBox::addItem() if you're on Windows and if you
	// actually have a floppy drive.
	//
	// The floppy drive will still click *one time* when user tries to type
	// anything, but at least it won't click on readSettings() anymore
	// (so, essentially, when user opens config box).
	if (!d->completerActive)
	{
		d->cbTargetDirectory->setCompleter(new QCompleter(new QDirModel()));
		d->completerActive = true;
	}
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
	gConfig.wadseeker.targetDirectory = d->targetDirectory();
	gConfig.wadseeker.connectTimeoutSeconds = d->spinConnectTimeout->value();
	gConfig.wadseeker.downloadTimeoutSeconds = d->spinDownloadTimeout->value();
	gConfig.wadseeker.maxConcurrentSiteDownloads = d->spinMaxConcurrentSiteSeeks->value();
	gConfig.wadseeker.maxConcurrentWadDownloads = d->spinMaxConcurrentWadDownloads->value();
}

void CFGWadseekerGeneral::showEvent(QShowEvent *event)
{
	activateCompleter();
	ConfigPage::showEvent(event);
}

ConfigPage::Validation CFGWadseekerGeneral::validate()
{
	QString error;

	QFileInfo targetDirectory = d->targetDirectory();
	if (error.isEmpty() && d->targetDirectory().isEmpty())
	{
		error = tr("No path specified.");
	}

	if (error.isEmpty() && !targetDirectory.exists())
	{
		error = tr("This path doesn't exist.");
	}

	if (error.isEmpty() && !targetDirectory.isDir())
	{
		error = tr("This is not a directory.");
	}

	if (error.isEmpty() && !targetDirectory.isWritable())
	{
		error = tr("This directory cannot be written to.");
	}

	// If path seems valid also take a look at the file
	// paths configuration. Warn if it is not on the list.
	if (error.isEmpty())
	{
		bool pathOnList = false;
		foreach(FileSearchPath possiblePath, gConfig.doomseeker.wadPaths)
		{
			// Bring paths to QFileInfo before string comparison. Two same paths
			// may have different string representations.
			// TODO: Consider recursive paths.
			QFileInfo possiblePathFileInfo(possiblePath.path());

			if (possiblePathFileInfo == targetDirectory)
			{
				pathOnList = true;
				break;
			}
		}

		if (!pathOnList)
		{
			error = tr(
				"The specified target directory for Wadseeker could not be found on the file (WAD) paths list.\n\n"
				"Doomseeker will automatically add this path to the file search paths.");
		}
	}

	d->lblDirectoryWarning->setVisible(!error.isEmpty());
	d->lblDirectoryWarning->setText(error);
	return error.isEmpty() ? VALIDATION_OK : VALIDATION_ERROR;
}
