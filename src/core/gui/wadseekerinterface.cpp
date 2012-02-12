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
	if(Main::mainWindow)
		((MainWindow*)(Main::mainWindow))->stopAutoRefreshTimer();
	setupUi(this);
	setStateWaiting();

	initMessageColors();

	this->setWindowIcon(QIcon(":/icon.png"));

	this->connect(btnClose, SIGNAL( clicked() ),
		SLOT( reject() ) );
	this->connect(btnDownload, SIGNAL( clicked() ),
		SLOT( accept() ) );
	this->connect(&updateTimer, SIGNAL( timeout() ),
		SLOT( registerUpdateRequest() ) );

	// Connect Wadseeker to the dialog box.
	this->connect(&wadseeker, SIGNAL( allDone(bool) ),
		SLOT( allDone(bool) ) );
	this->connect(&wadseeker, SIGNAL( message(const QString&, WadseekerLib::MessageType) ),
		SLOT( message(const QString&, WadseekerLib::MessageType) ) );
	this->connect(&wadseeker, SIGNAL( seekStarted(const QStringList&) ),
		SLOT( seekStarted(const QStringList&) ) );
	this->connect(&wadseeker, SIGNAL( fileInstalled(const QString&) ),
		SLOT( fileDownloadSuccessful(const QString&) ) );
	this->connect(&wadseeker, SIGNAL( siteFinished(const QUrl&) ),
		SLOT( siteFinished(const QUrl&) ) );
	this->connect(&wadseeker, SIGNAL( siteProgress(const QUrl&, qint64, qint64) ),
		SLOT( siteProgress(const QUrl&, qint64, qint64) ) );
	this->connect(&wadseeker, SIGNAL( siteRedirect(const QUrl&, const QUrl&) ),
		SLOT( siteRedirect(const QUrl&, const QUrl&) ) );
	this->connect(&wadseeker, SIGNAL( siteStarted(const QUrl&) ),
		SLOT( siteStarted(const QUrl&) ) );

	// Connect Wadseeker to the WADs table widget.
	twWads->connect(&wadseeker, SIGNAL( fileDownloadFinished(const QString&) ),
		SLOT( setFileDownloadFinished(const QString&) ) );
	twWads->connect(&wadseeker, SIGNAL( fileDownloadProgress(const QString&, qint64, qint64) ),
		SLOT( setFileProgress(const QString&, qint64, qint64) ) );
	twWads->connect(&wadseeker, SIGNAL( fileDownloadStarted(const QString&, const QUrl&) ),
		SLOT( setFileUrl(const QString&, const QUrl&) ) );
		
	// Connect tables.
	this->connect(twWads, SIGNAL( rightMouseClick(const QModelIndex&, const QPoint&) ),
		SLOT( wadsTableRightClicked(const QModelIndex&, const QPoint&) ) );

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
	if(Main::mainWindow)
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
	setStateWaiting();

	if (bSuccess)
	{
		displayMessage(tr("All done. Success."), WadseekerLib::NoticeImportant, false);

		if (bAutomatic)
		{
			// Close the dialog box.
			done(QDialog::Accepted);
		}
	}
	else
	{
		QStringList failures = unsuccessfulWads();

		foreach (const QString& failure, failures)
		{
			twWads->setFileFailed(failure);
		}

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

void WadseekerInterface::fileDownloadSuccessful(const QString& filename)
{
	successfulWads << filename;
	twWads->setFileSuccessful(filename);
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
	updateProgressBar();
	updateTitle();
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
	teWadseekerOutput->clear();
	pbOverallProgress->setValue(0);
	displayMessage("Seek started on filenames: " + filenames.join(", "), WadseekerLib::Notice, false);

	seekedWads = filenames;
	successfulWads.clear();
	twSites->setRowCount(0);
	twWads->setRowCount(0);
	setStateDownloading();

	foreach (const QString& name, filenames)
	{
		twWads->addFile(name);
	}
}

void WadseekerInterface::setStateDownloading()
{
	btnClose->setText(tr("Abort"));
	btnDownload->setEnabled(false);
	state = Downloading;
}

void WadseekerInterface::setStateWaiting()
{
	btnClose->setText(tr("Close"));
	btnDownload->setEnabled(true);
	state = Waiting;
}

void WadseekerInterface::setupIdgames()
{
	QString idgamesUrl = Wadseeker::defaultIdgamesUrl();
	bool useIdgames = true;
	bool idgamesHasHighPriority = false;

	useIdgames = gConfig.wadseeker.bSearchInIdgames;
	idgamesUrl = gConfig.wadseeker.idgamesURL;

	wadseeker.setIdgamesEnabled(useIdgames);
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
	twSites->removeUrl(site);
	displayMessage("Site finished: " + site.toEncoded(), WadseekerLib::Notice, false);
}

void WadseekerInterface::siteProgress(const QUrl& site, qint64 bytes, qint64 total)
{
	twSites->setUrlProgress(site, bytes, total);
}

void WadseekerInterface::siteRedirect(const QUrl& oldUrl, const QUrl& newUrl)
{
	twSites->removeUrl(oldUrl);
	twSites->addUrl(newUrl);
	displayMessage("Site redirect: " + oldUrl.toEncoded() + " -> " + newUrl.toEncoded(), WadseekerLib::Notice, false);
}

void WadseekerInterface::siteStarted(const QUrl& site)
{
	twSites->addUrl(site);
	displayMessage("Site started: " + site.toEncoded(), WadseekerLib::Notice, false);
}

void WadseekerInterface::startSeeking(const QStringList& seekedFilesList)
{
	if (seekedFilesList.isEmpty())
	{
		return;
	}

	// Get rid of the whitespace characters from each filename; we don't want
	// to be searching " awad.wad".
	QStringList seekedFilesListFormatted;
	foreach (QString filenameFormatted, seekedFilesList)
	{
		filenameFormatted = filenameFormatted.trimmed();

		seekedFilesListFormatted << filenameFormatted;
	}

	setupIdgames();

	wadseeker.setTargetDirectory(gConfig.wadseeker.targetDirectory);
	wadseeker.setCustomSite(customSite);
	wadseeker.setMaximumConcurrentSeeks(gConfig.wadseeker.maxConcurrentSiteDownloads);
	wadseeker.setMaximumConcurrentDownloads(gConfig.wadseeker.maxConcurrentWadDownloads);
	wadseeker.startSeek(seekedFilesListFormatted);
}

void WadseekerInterface::updateProgressBar()
{
	double totalPercentage = twWads->totalDonePercentage();
	unsigned progressBarValue = (unsigned)(totalPercentage * 100.0);
	
	pbOverallProgress->setValue(progressBarValue);
}

void WadseekerInterface::updateTitle()
{
	switch (state)
	{
		case Downloading:
		{
			double totalPercentage = twWads->totalDonePercentage();
			if (totalPercentage < 0.0)
			{
				totalPercentage = 0.0;
			}

			setWindowTitle(tr("[%1%] Wadseeker").arg(totalPercentage, 6, 'f', 2));
			break;
		}

		default:
		case Waiting:
			resetTitleToDefault();
			break;
	}
}

QStringList	WadseekerInterface::unsuccessfulWads() const
{
	QStringList allWads = seekedWads;

	foreach (const QString& success, successfulWads)
	{
		allWads.removeAll(success);
	}

	return allWads;
}

void WadseekerInterface::wadsTableRightClicked(const QModelIndex& index, const QPoint& cursorPosition)
{
	WadseekerWadsTable::ContextMenu* menu = twWads->contextMenu(index, cursorPosition);
	
	// Disable actions depending on Wadseeker's state.
	QString fileName = twWads->fileNameAtRow(index.row());
	if (!wadseeker.isDownloadingFile(fileName))
	{
		menu->actionSkipCurrentSite->setEnabled(false);
	}
	
	QAction* pResult = menu->exec();
	
	if (pResult == menu->actionSkipCurrentSite)
	{
		QString wadName = twWads->fileNameAtRow(index.row());
		twWads->setFileUrl(fileName, QUrl());
		
		wadseeker.skipFileCurrentUrl(wadName);
	}
	else if (pResult != NULL)
	{
		QMessageBox::warning(this, tr("Context Menu error"), tr("Unknown action selected"));
	}
	
	delete menu;
}
