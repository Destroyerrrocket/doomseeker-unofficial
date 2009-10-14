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
#include "mainwindow.h"
#include "main.h"

WadSeekerInterface::WadSeekerInterface(QWidget* parent) : QDialog(parent)
{
	((MainWindow*)(Main::mainWindow))->stopAutoRefreshTimer();
	setupUi(this);
	setStateWaiting();

	connect(&wadseeker, SIGNAL( aborted() ), this, SLOT( aborted() ) );
	connect(&wadseeker, SIGNAL( allDone() ), this, SLOT( allDone() ) );
	connect(&wadseeker, SIGNAL( downloadProgress(int, int) ), this, SLOT( downloadProgress(int, int) ) );
	connect(&wadseeker, SIGNAL( message(const QString&, Wadseeker::MessageType) ), this, SLOT( message(const QString&, Wadseeker::MessageType) ) );

	bAutomatic = false;
	bFirstShown = false;

	if (Main::config->settingExists("WadseekerSearchURLs"))
	{
		SettingsData* setting = Main::config->setting("WadseekerSearchURLs");
		QStringList urlList;
		QStringList strLst = setting->string().split(";");
		QStringList::iterator it;
		for (it = strLst.begin(); it != strLst.end(); ++it)
		{
			urlList << QUrl::fromPercentEncoding(it->toAscii());
		}

		wadseeker.setPrimarySites(urlList);
	}
	else
	{
		// Theoreticaly this else should never happen due to config initialization in main.cpp.
		// theoreticaly...
		wadseeker.setPrimarySitesToDefault();
	}

	Main::config->createSetting("WadseekerSearchInIdgames", true);
	Main::config->createSetting("WadseekerIdgamesPriority", 0); // 0 == After all other sites
	Main::config->createSetting("WadseekerIdgamesURL", Wadseeker::defaultIdgamesUrl());

	QString idgamesURL = Wadseeker::defaultIdgamesUrl();
	bool useIdgames = true, idgamesHasHighPriority = false;
	if (Main::config->settingExists("WadseekerSearchInIdgames"))
		useIdgames = Main::config->setting("WadseekerSearchInIdgames")->integer();
	if (Main::config->settingExists("WadseekerIdgamesPriority"))
		idgamesHasHighPriority = Main::config->setting("WadseekerIdgamesPriority")->integer();
	if (Main::config->settingExists("WadseekerIdgamesURL"))
		idgamesURL = Main::config->setting("WadseekerIdgamesURL")->string();

	wadseeker.setUseIdgames(useIdgames, idgamesHasHighPriority, idgamesURL);
}

WadSeekerInterface::~WadSeekerInterface()
{
	((MainWindow*)(Main::mainWindow))->initAutoRefreshTimer();
}

void WadSeekerInterface::aborted()
{
	teWadseekerOutput->append(tr("Aborted!"));
	fail();

	this->setStateWaiting();
}

void WadSeekerInterface::accept()
{
	if (leWadName->text().isEmpty())
		return;

	QStringList list;
	list << leWadName->text();

	startSeeking(list);
}

void WadSeekerInterface::allDone()
{
	teWadseekerOutput->append(tr("All done."));
	setStateWaiting();
	if (wadseeker.areAllFilesFound())
	{
		teWadseekerOutput->append(tr("SUCCESS!"));
		if (bAutomatic)
		{
			this->done(Accepted);
		}
	}
	else
	{
		fail();
		teWadseekerOutput->append(tr("FAIL!"));
	}
}

void WadSeekerInterface::downloadProgress(int done, int total)
{
	pbProgress->setMaximum(total);
	pbProgress->setValue(done);
}

void WadSeekerInterface::fail()
{
	bAutomatic = false;
	const QStringList& notFoundWads = wadseeker.filesNotFound();
	QString nfwStr = tr("Following files were not found: %1").arg(notFoundWads.join(", "));
	teWadseekerOutput->append(nfwStr);

	pbProgress->setMaximum(100);
	pbProgress->setValue(0);
}

void WadSeekerInterface::message(const QString& msg, Wadseeker::MessageType type)
{
	QString str;
	switch (type)
	{
		case Wadseeker::CriticalError:
			str = tr("CRITICAL ERROR: %1").arg(msg);
			setStateWaiting();
			break;

		case Wadseeker::Error:
			str = tr("Error: %1").arg(msg);
			break;

		case Wadseeker::Notice:
			str = msg;
			break;
	}

	teWadseekerOutput->append(str);
}

void WadSeekerInterface::reject()
{
	switch(state)
	{
		case DOWNLOADING:
			wadseeker.abort();
			break;

		case WAITING:
			this->done(Rejected);
			break;
	}
}

void WadSeekerInterface::setStateDownloading()
{
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
	buttonBox->button(QDialogButtonBox::Close)->setEnabled(false);
	state = DOWNLOADING;
}

void WadSeekerInterface::setStateWaiting()
{
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
	buttonBox->button(QDialogButtonBox::Close)->setEnabled(true);
	state = WAITING;
}

void WadSeekerInterface::showEvent(QShowEvent* event)
{
	if (bAutomatic && !bFirstShown)
	{
		bFirstShown = true;
		startSeeking(seekedWads);
	}
}

void WadSeekerInterface::startSeeking(const QStringList& seekedFilesList)
{
	if (seekedFilesList.isEmpty())
		return;

	teWadseekerOutput->clear();

	SettingsData* targetDir, *connectTimeout, *downloadTimeout;
	targetDir = Main::config->setting("WadseekerTargetDirectory");
	connectTimeout = Main::config->setting("WadseekerConnectTimeoutSeconds");
	downloadTimeout = Main::config->setting("WadseekerDownloadTimeoutSeconds");

	setStateDownloading();

	wadseeker.setTimeConnectTimeout(connectTimeout->integer());
	wadseeker.setTimeDownloadTimeout(downloadTimeout->integer());
	wadseeker.setTargetDirectory(targetDir->string());
	wadseeker.seekWads(seekedFilesList);
}
