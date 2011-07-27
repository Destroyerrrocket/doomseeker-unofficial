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
#include "configuration/doomseekerconfig.h"
#include "mainwindow.h"
#include "main.h"
#include "strings.h"

#include <QMessageBox>

const int WadseekerInterface::UPDATE_INTERVAL_MS = 500;

WadseekerInterface::WadseekerInterface(QWidget* parent)
: QDialog(parent)
{
	bNeedsUpdate = false;

	((MainWindow*)(Main::mainWindow))->stopAutoRefreshTimer();
	setupUi(this);
	setStateWaiting();

	initMessageColors();

	this->setWindowIcon(QIcon(":/icon.png"));

	connect(btnClose, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect(btnDownload, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect(btnStop, SIGNAL( clicked() ), &wadseeker, SLOT( abort() ) );
	connect(btnSkipSite, SIGNAL( clicked() ), &wadseeker, SLOT( skipSite() ) );
	connect(&updateTimer, SIGNAL( timeout() ), this, SLOT( registerUpdateRequest() ) );

	this->connect(&wadseeker, SIGNAL( allDone(bool) ),
		SLOT( allDone(bool) ) );
	this->connect(&wadseeker, SIGNAL( message(const QString&, WadseekerLib::MessageType) ),
		SLOT( message(const QString&, WadseekerLib::MessageType) ) );
	this->connect(&wadseeker, SIGNAL( seekStarted(const QStringList&) ),
		SLOT( seekStarted(const QStringList&) ) );
	this->connect(&wadseeker, SIGNAL( siteFinished(const QUrl&) ),
		SLOT( siteFinished(const QUrl&) ) );
	this->connect(&wadseeker, SIGNAL( siteProgress(const QUrl&, qint64, qint64) ),
		SLOT( siteProgress(const QUrl&, qint64, qint64) ) );
	this->connect(&wadseeker, SIGNAL( siteRedirect(const QUrl&, const QUrl&) ),
		SLOT( siteRedirect(const QUrl&, const QUrl&) ) );
	this->connect(&wadseeker, SIGNAL( siteStarted(const QUrl&) ),
		SLOT( siteStarted(const QUrl&) ) );

	bAutomatic = false;
	bFirstShown = false;

	const QStringList& urlList = gConfig.wadseeker.searchURLs;

	if (!urlList.isEmpty())
	{
		wadseeker.setPrimarySites(urlList);
	}
	else
	{
		wadseeker.setPrimarySitesToDefault();
	}

	updateTimer.setSingleShot(false);
	updateTimer.start(UPDATE_INTERVAL_MS);
}

WadseekerInterface::~WadseekerInterface()
{
	((MainWindow*)(Main::mainWindow))->initAutoRefreshTimer();
}

void WadseekerInterface::accept()
{
	if (leWadName->text().isEmpty())
		return;

	startSeeking(leWadName->text().split(',', QString::SkipEmptyParts));
}

void WadseekerInterface::allDone(bool bSuccess)
{
	if (bSuccess)
	{
		displayMessage(tr("All done. Success."), WadseekerLib::NoticeImportant, false);
	}
	else
	{
		displayMessage(tr("All done. Fail."), WadseekerLib::CriticalError, false);
	}

}

void WadseekerInterface::displayMessage(const QString& message, WadseekerLib::MessageType type, bool bPrependErrorsWithMessageType)
{
	QString strProcessedMessage;

	bool bPrependWithNewline = false;
	QString wrapHtmlLeft = "<div style=\"%1\">";
	QString wrapHtmlRight = "</div>";
	QString htmlStyle;

	switch (type)
	{
		case WadseekerLib::CriticalError:
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

		case WadseekerLib::Error:
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

		case WadseekerLib::Notice:
			htmlStyle = QString("color: %1;").arg(colorHtmlMessageNotice);

			strProcessedMessage = message;
			break;

		case WadseekerLib::NoticeImportant:
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

void WadseekerInterface::downloadProgress(int done, int total)
{
	if (bNeedsUpdate)
	{

	}
}

void WadseekerInterface::fail()
{

}

void WadseekerInterface::initMessageColors()
{
	colorHtmlMessageNotice = gConfig.wadseeker.colorMessageNotice;
	colorHtmlMessageError = gConfig.wadseeker.colorMessageError;
	colorHtmlMessageFatalError = gConfig.wadseeker.colorMessageCriticalError;
}

void WadseekerInterface::message(const QString& message, WadseekerLib::MessageType type)
{
	displayMessage(message, type, true);
}

void WadseekerInterface::registerUpdateRequest()
{
	bNeedsUpdate = true;
}

void WadseekerInterface::reject()
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

void WadseekerInterface::resetTitleToDefault()
{
    setWindowTitle(tr("Wadseeker"));
}

void WadseekerInterface::seekStarted(const QStringList& filenames)
{
	displayMessage("Seek started on filenames: " + filenames.join(", "), WadseekerLib::Notice, false);
}

void WadseekerInterface::setStateDownloading()
{
	btnClose->setEnabled(false);
	btnDownload->setEnabled(false);
	btnStop->setEnabled(true);
	btnSkipSite->setEnabled(true);

	state = Downloading;
	btnSkipSite->setDefault(true);
}

void WadseekerInterface::setStateWaiting()
{
	btnClose->setEnabled(true);
	btnDownload->setEnabled(true);
	btnStop->setEnabled(false);
	btnSkipSite->setEnabled(false);

	state = Waiting;
	btnDownload->setDefault(true);

	lblTimeUntilArrival->setText("N/A");
}

void WadseekerInterface::setupIdgames()
{
	QString idgamesUrl = Wadseeker::defaultIdgamesUrl();
	bool useIdgames = true;
	bool idgamesHasHighPriority = false;

	useIdgames = gConfig.wadseeker.bSearchInIdgames;
	idgamesHasHighPriority = gConfig.wadseeker.idgamesPriority != 0;
	idgamesUrl = gConfig.wadseeker.idgamesURL;

	wadseeker.setIdgamesEnabled(useIdgames);
	wadseeker.setIdgamesHighPriority(idgamesHasHighPriority);
	wadseeker.setIdgamesUrl(idgamesUrl);
}

void WadseekerInterface::showEvent(QShowEvent* event)
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

void WadseekerInterface::siteFinished(const QUrl& site)
{
	displayMessage("Site finished: " + site.toEncoded(), WadseekerLib::Notice, false);
}

void WadseekerInterface::siteProgress(const QUrl& site, qint64 bytes, qint64 total)
{

}

void WadseekerInterface::siteRedirect(const QUrl& oldUrl, const QUrl& newUrl)
{
	displayMessage("Site redirect: " + oldUrl.toEncoded() + " -> " + newUrl.toEncoded(), WadseekerLib::Notice, false);
}

void WadseekerInterface::siteStarted(const QUrl& site)
{
	displayMessage("Site started: " + site.toEncoded(), WadseekerLib::Notice, false);
}

void WadseekerInterface::startSeeking(const QStringList& seekedFilesList)
{
	if (seekedFilesList.isEmpty())
	{
		return;
	}

	teWadseekerOutput->clear();

	// Get rid of the whitespace characters from each filename; we don't want
	// to be searching " awad.wad".
	QStringList seekedFilesListFormatted;
	foreach (QString filenameFormatted, seekedFilesList)
	{
		filenameFormatted = filenameFormatted.trimmed();

		seekedFilesListFormatted << filenameFormatted;
	}

	setupIdgames();

	// TODO
	//wadseeker.setCustomSite("a");
	wadseeker.setTargetDirectory(gConfig.wadseeker.targetDirectory);
	wadseeker.startSeek(seekedFilesListFormatted);
}

void WadseekerInterface::updateTitle()
{
    switch (state)
    {
        case Downloading:
            {

            }
            break;

        default:
            resetTitleToDefault();
            break;
    }
}
