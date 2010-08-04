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
#include "strings.h"

#include <QMessageBox>

const int WadSeekerInterface::UPDATE_INTERVAL_MS = 500;

void WadSeekerInterface::initMessageColors()
{
	colorHtmlMessageNotice = *config["ColorMessageNotice"];
	colorHtmlMessageError = *config["ColorMessageError"];
	colorHtmlMessageFatalError = *config["ColorMessageCriticalError"];
}

WadSeekerInterface::WadSeekerInterface(QWidget* parent)
: QDialog(parent), config(Ini::nullSection)
{
	config = Main::ini->createSection("Wadseeker");

	bNeedsUpdate = false;

	((MainWindow*)(Main::mainWindow))->stopAutoRefreshTimer();
	setupUi(this);
	setStateWaiting();
	
	initMessageColors();
	
	connect(btnClose, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect(btnDownload, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect(btnStop, SIGNAL( clicked() ), &wadseeker, SLOT( abort() ) );
	connect(btnSkipSite, SIGNAL( clicked() ), &wadseeker, SLOT( skipSite() ) );
	connect(&updateTimer, SIGNAL( timeout() ), this, SLOT( registerUpdateRequest() ) );

	connect(&wadseeker, SIGNAL( aborted() ), this, SLOT( aborted() ) );
	connect(&wadseeker, SIGNAL( allDone() ), this, SLOT( allDone() ) );
	connect(&wadseeker, SIGNAL( downloadProgress(int, int) ), this, SLOT( downloadProgress(int, int) ) );
	connect(&wadseeker, SIGNAL( message(const QString&, Wadseeker::MessageType) ), this, SLOT( message(const QString&, Wadseeker::MessageType) ) );

	bAutomatic = false;
	bFirstShown = false;

	if (!config.retrieveSetting("SearchURLs").isNull())
	{
		QStringList urlList;
		QStringList strLst = config["SearchURLs"]->split(";");
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

	setupIdgames();

	updateTimer.setSingleShot(false);
	updateTimer.start(UPDATE_INTERVAL_MS);
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

	startSeeking(leWadName->text().split(',', QString::SkipEmptyParts));
}

void WadSeekerInterface::allDone()
{
	displayMessage(tr("All done."), Wadseeker::NoticeImportant, false);
	
	setStateWaiting();
	if (wadseeker.areAllFilesFound())
	{
		displayMessage(tr("SUCCESS!"), Wadseeker::Notice, false);
		if (bAutomatic)
		{
			this->done(Accepted);
		}
	}
	else
	{
		fail();
		
	}
	
	
}

void WadSeekerInterface::displayMessage(const QString& message, Wadseeker::MessageType type, bool bPrependErrorsWithMessageType)
{
	QString strProcessedMessage;
	
	bool bPrependWithNewline = false;
	QString wrapHtmlLeft = "<div style=\"%1\">";
	QString wrapHtmlRight = "</div>";
	QString htmlStyle;
	
	switch (type)
	{
		case Wadseeker::CriticalError:
			htmlStyle = QString("color: %1; font-weight: bold;").arg(colorHtmlMessageFatalError);
			bPrependWithNewline = true;
			
			if (bPrependErrorsWithMessageType)
			{
				strProcessedMessage = tr("CRITICAL ERROR: %1").arg(message);
			}
			else
			{
				strProcessedMessage = message;
			}
		
			setStateWaiting();
			break;

		case Wadseeker::Error:
			htmlStyle = QString("color: %1;").arg(colorHtmlMessageError);
			
			if (bPrependErrorsWithMessageType)
			{
				strProcessedMessage = tr("Error: %1").arg(message);
			}
			else
			{
				strProcessedMessage = message;
			}
			break;

		case Wadseeker::Notice:
			htmlStyle = QString("color: %1;").arg(colorHtmlMessageNotice);
			
			strProcessedMessage = message;
			break;
			
		case Wadseeker::NoticeImportant:
			htmlStyle = QString("color: %1; font-weight: bold;").arg(colorHtmlMessageNotice);
			bPrependWithNewline = true;
			
			strProcessedMessage = message;
			break;
	}
	
	if (bPrependWithNewline && !teWadseekerOutput->toPlainText().isEmpty())
	{			
		strProcessedMessage = "<br>" + strProcessedMessage;
	}
	
	wrapHtmlLeft = wrapHtmlLeft.arg(htmlStyle);
	
	strProcessedMessage = wrapHtmlLeft + strProcessedMessage + wrapHtmlRight;

	teWadseekerOutput->append(strProcessedMessage);
}

void WadSeekerInterface::downloadProgress(int done, int total)
{
	if (bNeedsUpdate)
	{
		float speed = wadseeker.downloadSpeed();
		float estimatedTimeUntilArrival = wadseeker.estimatedTimeUntilArrivalOfCurrentFile();

		if (speed >= 0.0f)
		{
			lblCurrentSpeed->setText( Strings::formatDataSpeed(speed) );
		}
		else
		{
			lblCurrentSpeed->setText( "N/A" );
		}

		if (estimatedTimeUntilArrival >= 0.0f)
		{
			lblTimeUntilArrival->setText( Strings::formatTime(estimatedTimeUntilArrival) );
		}
		else
		{
			lblTimeUntilArrival->setText( "N/A" );
		}
		
		if (total > 0)
		{
			QString strDone = Strings::formatDataAmount(done);
			QString strTotal = Strings::formatDataAmount(total);
			lblDataAmount->setText( strDone + " / " + strTotal );
		}
		else
		{
			lblDataAmount->setText( "N/A" );
		}

		bNeedsUpdate = false;
	}

	pbProgress->setMaximum(total);
	pbProgress->setValue(done);
}

void WadSeekerInterface::fail()
{
	bAutomatic = false;
	const QStringList& notFoundWads = wadseeker.filesNotFound();
	
	displayMessage(tr("FAIL!"), Wadseeker::CriticalError, false);
	QString notFoundWadsString = tr("Following files were not found: %1").arg(notFoundWads.join(", "));
	message(notFoundWadsString, Wadseeker::Error);

	pbProgress->setMaximum(100);
	pbProgress->setValue(0);
}

void WadSeekerInterface::message(const QString& message, Wadseeker::MessageType type)
{
	displayMessage(message, type, true);
}

void WadSeekerInterface::registerUpdateRequest()
{
	bNeedsUpdate = true;
}

void WadSeekerInterface::reject()
{
	switch(state)
	{
		case Downloading:
			wadseeker.abort();
			break;

		case Waiting:
			this->done(Rejected);
			break;
	}
}

void WadSeekerInterface::setStateDownloading()
{
	btnClose->setEnabled(false);
	btnDownload->setEnabled(false);
	btnStop->setEnabled(true);
	btnSkipSite->setEnabled(true);

	state = Downloading;
	btnSkipSite->setDefault(true);
}

void WadSeekerInterface::setStateWaiting()
{
	btnClose->setEnabled(true);
	btnDownload->setEnabled(true);
	btnStop->setEnabled(false);
	btnSkipSite->setEnabled(false);

	state = Waiting;
	btnDownload->setDefault(true);
	
	lblTimeUntilArrival->setText("N/A");
}

void WadSeekerInterface::setupIdgames()
{
	QString idgamesURL = Wadseeker::defaultIdgamesUrl();
	bool useIdgames = true;
	bool idgamesHasHighPriority = false;

	config.createSetting("SearchInIdgames", true);
	config.createSetting("IdgamesPriority", 0); // 0 == After all other sites
	config.createSetting("IdgamesURL", Wadseeker::defaultIdgamesUrl());

	useIdgames = config["SearchInIdgames"];
	idgamesHasHighPriority = config["IdgamesPriority"];
	idgamesURL = *config["IdgamesURL"];

	wadseeker.setUseIdgames(useIdgames, idgamesHasHighPriority, idgamesURL);
}

void WadSeekerInterface::showEvent(QShowEvent* event)
{
	if (!bFirstShown)
	{
		bFirstShown = true;
		
		if (bAutomatic)
		{
			startSeeking(seekedWads);
		}
	}
}

void WadSeekerInterface::startSeeking(const QStringList& seekedFilesList)
{
	if (seekedFilesList.isEmpty())
		return;

	teWadseekerOutput->clear();
	
	// Get rid of the whitespace characters from each filename; we don't want
	// to be searching " awad.wad".
	QStringList seekedFilesListFormatted;
	foreach (QString filenameFormatted, seekedFilesList)
	{
		filenameFormatted = filenameFormatted.trimmed();
	
		seekedFilesListFormatted << filenameFormatted;
	}

	setStateDownloading();

	wadseeker.setTimeConnectTimeout(config["ConnectTimeoutSeconds"]);
	wadseeker.setTimeDownloadTimeout(config["DownloadTimeoutSeconds"]);
	wadseeker.setTargetDirectory(config["TargetDirectory"]);
	wadseeker.seekWads(seekedFilesListFormatted);
}
