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
	setupUi(this);
	connect(&wadseeker, SIGNAL( allDone() ), this, SLOT( allDone() ) );
	connect(&wadseeker, SIGNAL( error(const QString&, bool) ), this, SLOT( error(const QString&, bool)) );
	connect(&wadseeker, SIGNAL( notice(const QString&) ), this, SLOT( notice(const QString&)) );
	connect(&wadseeker, SIGNAL( wadDone(bool, const QString&) ), this, SLOT( wadDone(bool, const QString&) ) );
	connect(&wadseeker, SIGNAL( wadSize(unsigned int) ), this, SLOT( wadSize(unsigned int) ) );
	connect(&wadseeker, SIGNAL( wadCurrentDownloadedSize(unsigned int, unsigned int) ), this, SLOT( wadCurrentDownloadedSize(unsigned int, unsigned int) ) );
	setStateWaiting();

	// Set site links
	if (Main::config->settingExists("WadseekerSearchURLs"))
	{
		SettingsData* setting = Main::config->setting("WadseekerSearchURLs");
		QList<QUrl> urlList;
		QStringList strLst = setting->string().split(";");
		QStringList::iterator it;
		for (it = strLst.begin(); it != strLst.end(); ++it)
		{
			urlList << QUrl::fromPercentEncoding(it->toAscii());
		}

		wadseeker.setGlobalSiteLinks(urlList);
	}
	else
	{
		// Theoreticaly this else should never happen due to config initialization in Main.cpp.
		// theoreticaly...
		wadseeker.setGlobalSiteLinksToDefaults();
	}
}

void WadSeekerInterface::accept()
{
	if (leWadName->text().isEmpty())
		return;

	teWadseekerOutput->clear();

	SettingsData* setting;
	setting = Main::config->setting("WadseekerTargetDirectory");

	setStateDownloading();

	QStringList list;
	list << leWadName->text();
	//list << "doom.wad" << "hexdd.wad" << "test.wad";
	wadseeker.setTargetDirectory(setting->string());
	wadseeker.seekWads(list);
}

void WadSeekerInterface::allDone()
{
	teWadseekerOutput->append(tr("All done."));
	setStateWaiting();
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
			wadseeker.abort();
			teWadseekerOutput->append(tr("Aborted!"));
			this->setStateWaiting();
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


