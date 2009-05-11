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
#include <QPushButton>

WadSeekerInterface::WadSeekerInterface(QWidget* parent) : QDialog(parent)
{
	bAutomaticStart = false;

	setupUi(this);
	connect(&pWadseeker, SIGNAL( aborted() ), this, SLOT( aborted() ) );
	connect(&pWadseeker, SIGNAL( allDone() ), this, SLOT( allDone() ) );
	connect(&pWadseeker, SIGNAL( error(const QString&, bool) ), this, SLOT( error(const QString&, bool)) );
	connect(&pWadseeker, SIGNAL( notice(const QString&) ), this, SLOT( notice(const QString&)) );
	connect(&pWadseeker, SIGNAL( wadDone(bool, const QString&) ), this, SLOT( wadDone(bool, const QString&) ) );
	connect(&pWadseeker, SIGNAL( wadSize(unsigned int) ), this, SLOT( wadSize(unsigned int) ) );
	connect(&pWadseeker, SIGNAL( wadCurrentDownloadedSize(unsigned int, unsigned int) ), this, SLOT( wadCurrentDownloadedSize(unsigned int, unsigned int) ) );
	setStateWaiting();

	this->bAutomaticCloseOnSuccess = false;

	// Set site links
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

		pWadseeker.setGlobalSiteLinks(urlList);
	}
	else
	{
		// Theoreticaly this else should never happen due to config initialization in Main.cpp.
		// theoreticaly...
		pWadseeker.setGlobalSiteLinksToDefaults();
	}
}

void WadSeekerInterface::aborted()
{
	teWadseekerOutput->append(tr("Aborted!"));
	bAutomaticCloseOnSuccess = false;
	const QStringList& notFoundWads = pWadseeker.notFoundWadsList();
	QString nfwStr = tr("Following files were not found: %1").arg(notFoundWads.join(" "));
	teWadseekerOutput->append(nfwStr);

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
	if (pWadseeker.areAllWadsFound())
	{
		teWadseekerOutput->append(tr("SUCCESS!"));
		if (bAutomaticCloseOnSuccess)
		{
			this->done(Accepted);
		}
	}
	else
	{
		bAutomaticCloseOnSuccess = false;
		const QStringList& notFoundWads = pWadseeker.notFoundWadsList();
		QString nfwStr = tr("Following files were not found: %1").arg(notFoundWads.join(" "));
		teWadseekerOutput->append(nfwStr);
		teWadseekerOutput->append(tr("FAIL!"));
	}
}

void WadSeekerInterface::error(const QString& err, bool bIsCritical)
{
	QString str;
	if (bIsCritical)
	{
		str = tr("CRITICAL ERROR: %1").arg(err);
		setStateWaiting();
	}
	else
	{
		str = tr("Error: %1").arg(err);
	}
	teWadseekerOutput->append(str);
}

void WadSeekerInterface::notice(const QString& str)
{
	teWadseekerOutput->append(str);
}

void WadSeekerInterface::reject()
{
	switch(state)
	{
		case DOWNLOADING:
			pWadseeker.abort();
			break;

		case WAITING:
			this->done(Rejected);
			break;
	}
}

void WadSeekerInterface::setAutomaticStart(const QStringList& seekedFilesList)
{
	if (!seekedFilesList.isEmpty())
	{
		bAutomaticStart = true;
		startSeeking(seekedFilesList);
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

void WadSeekerInterface::startSeeking(const QStringList& seekedFilesList)
{
	if (seekedFilesList.isEmpty())
		return;

	teWadseekerOutput->clear();

	SettingsData* setting;
	setting = Main::config->setting("WadseekerTargetDirectory");

	setStateDownloading();

	pWadseeker.setTargetDirectory(setting->string());
	pWadseeker.seekWads(seekedFilesList);
}

void WadSeekerInterface::wadDone(bool bFound, const QString& wadname)
{
	QString str = tr("File %1 done! Found: %2.\n").arg(wadname);
	if (bFound)
	{
		str = str.arg(tr("true"));
	}
	else
	{
		str = str.arg(tr("false"));
	}
	teWadseekerOutput->append(str);
}

void WadSeekerInterface::wadSize(unsigned int s)
{
	QString str = tr("Size: %1 B\n").arg(s);
	teWadseekerOutput->append(str);
	if (s == 0)
		s = 1;
	pbProgress->setMaximum(s);
}

void WadSeekerInterface::wadCurrentDownloadedSize(unsigned int howMuchSum, unsigned int percent)
{
	pbProgress->setValue(howMuchSum);
}


