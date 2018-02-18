//------------------------------------------------------------------------------
// missingwadsdialog.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "missingwadsdialog.h"

#include "ui_missingwadsdialog.h"
#include "gui/mainwindow.h"
#include "gui/wadseekerinterface.h"
#include "serverapi/serverstructs.h"
#include "application.h"
#include <wadseeker/freedoom.h>
#include <wadseeker/wadseeker.h>
#include <QListWidgetItem>

DClass<MissingWadsDialog> : public Ui::MissingWadsDialog
{
public:
	::MissingWadsDialog::MissingWadsProceed decision;
	QList<PWad> wads;
};
DPointeredNoCopy(MissingWadsDialog)

MissingWadsDialog::MissingWadsDialog(const QList<PWad> &wads, QWidget *parent)
: QDialog(parent)
{
	d->setupUi(this);
	d->decision = Cancel;
	d->wads = wads;

	setup();
	adjustSize();
}

MissingWadsDialog::~MissingWadsDialog()
{
}

void MissingWadsDialog::setAllowIgnore(bool allow)
{
	d->btnIgnore->setVisible(allow);
	d->lblUseIgnore->setVisible(allow);
	d->lblUseIgnoreCantRun->setVisible(allow);
}

void MissingWadsDialog::setup()
{
	d->btnInstallFreedoom->hide();
	if (WadseekerInterface::isInstantiated())
	{
		setupWadseekerIsRunning();
	}
	else
	{
		setupWadseekerNotRunning();
	}
}

void MissingWadsDialog::setupWadseekerIsRunning()
{
	d->areaWadseekerAlreadyRunning->show();
	d->areaWadseekerCanBeRun->hide();
	d->btnInstall->hide();
}

void MissingWadsDialog::setupWadseekerNotRunning()
{
	d->areaWadseekerAlreadyRunning->hide();
	d->areaWadseekerCanBeRun->show();

	setupForbiddenFilesArea();
	setupDownloadableFilesArea();
	setupOptionalFilesArea();

	d->btnInstall->setVisible(hasAnyAllowedFile());
}

void MissingWadsDialog::setupForbiddenFilesArea()
{
	QStringList files = forbiddenFiles();
	if (!files.isEmpty())
	{
		d->areaCantBeDownloaded->show();
		d->lblCantBeDownloadedFiles->setText(files.join(", "));

		bool installFreedoom = isFreedoomReplaceableOnList(files);
		d->lblInstallFreedoom->setVisible(installFreedoom);
		d->btnInstallFreedoom->setVisible(installFreedoom);
	}
	else
	{
		d->areaCantBeDownloaded->hide();
	}
}

void MissingWadsDialog::setupDownloadableFilesArea()
{
	QStringList files = downloadableFiles();
	if (!files.isEmpty())
	{
		d->areaCanBeDownloadedFiles->show();
		d->lblCanBeDownloadedFiles->setText(files.join(", "));
	}
	else
	{
		d->areaCanBeDownloadedFiles->hide();
	}
}

void MissingWadsDialog::setupOptionalFilesArea()
{
	QStringList files = optionalFiles();
	if (!files.isEmpty())
	{
		d->areaOptionalFiles->show();
		foreach (const QString &file, files)
		{
			QListWidgetItem *item = new QListWidgetItem(file, d->optionalFilesList);
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
			item->setCheckState(Qt::Checked);
		}
	}
	else
	{
		d->areaOptionalFiles->hide();
	}
}

bool MissingWadsDialog::isFreedoomReplaceableOnList(const QStringList &files) const
{
	foreach (const QString &file, files)
	{
		if (Freedoom::hasFreedoomReplacement(file))
		{
			return true;
		}
	}
	return false;
}

void MissingWadsDialog::updateStateAccordingToFileSelection()
{
	d->btnInstall->setEnabled(!filesToDownload().isEmpty());
}

void MissingWadsDialog::ignoreMissingFiles()
{
	d->decision = Ignore;
	accept();
}

void MissingWadsDialog::installFreedoom()
{
	QTimer::singleShot(0, gApp->mainWindow(), SLOT(showInstallFreedoomDialog()));
	accept();
}

void MissingWadsDialog::installMissingFiles()
{
	d->decision = Install;
	accept();
}

QStringList MissingWadsDialog::downloadableFiles() const
{
	QStringList result;
	foreach (const PWad &file, d->wads)
	{
		if (!Wadseeker::isForbiddenWad(file.name()) && !file.isOptional())
		{
			result << file.name();
		}
	}
	return result;
}

QStringList MissingWadsDialog::forbiddenFiles() const
{
	QStringList result;
	foreach (const PWad &file, d->wads)
	{
		if (Wadseeker::isForbiddenWad(file.name()))
		{
			result << file.name();
		}
	}
	return result;
}

QStringList MissingWadsDialog::optionalFiles() const
{
	QStringList result;
	foreach (const PWad &file, d->wads)
	{
		if (!Wadseeker::isForbiddenWad(file.name()) && file.isOptional())
		{
			result << file.name();
		}
	}
	return result;
}

QStringList MissingWadsDialog::filesToDownload() const
{
	QStringList result = downloadableFiles();
	result << selectedOptionalFiles();
	return result;
}

QStringList MissingWadsDialog::selectedOptionalFiles() const
{
	QStringList result;
	for (int i = 0; i < d->optionalFilesList->count(); ++i)
	{
		QListWidgetItem *item = d->optionalFilesList->item(i);
		if (item->checkState() == Qt::Checked)
		{
			result << item->text();
		}
	}
	return result;
}

bool MissingWadsDialog::hasAnyAllowedFile() const
{
	return !downloadableFiles().isEmpty() || !optionalFiles().isEmpty();
}

MissingWadsDialog::MissingWadsProceed MissingWadsDialog::decision() const
{
	return d->decision;
}
