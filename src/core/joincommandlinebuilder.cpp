//------------------------------------------------------------------------------
// joincommandlinebuilder.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "joincommandlinebuilder.h"

#include "apprunner.h"
#include "datapaths.h"
#include "log.h"
#include "configuration/doomseekerconfig.h"
#include "gui/passworddlg.h"
#include "gui/wadseekerinterface.h"
#include "gui/wadseekershow.h"
#include "ini/settingsproviderqt.h"
#include "plugins/engineplugin.h"
#include "serverapi/exefile.h"
#include "serverapi/gameclientrunner.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "application.h"
#include "gamedemo.h"

#include <wadseeker/wadseeker.h>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <cassert>

DClass<JoinCommandLineBuilder>
{
	public:
		CommandLineInfo cli;
		bool configurationError;
		QString connectPassword;
		QString error;
		GameDemo demo;
		QString demoName;
		QString inGamePassword;
		ServerPtr server;
		QWidget *parentWidget;
		bool passwordsAlreadySet;

		// For missing wads dialog
		QDialogButtonBox *buttonBox;
		QDialogButtonBox::StandardButton lastButtonClicked;
};

DPointered(JoinCommandLineBuilder)

JoinCommandLineBuilder::JoinCommandLineBuilder(ServerPtr server,
	GameDemo demo, QWidget *parentWidget)
{
	d->configurationError = false;
	d->demo = demo;
	d->demoName = GameDemo::mkDemoFullPath(demo, *server->plugin());
	d->parentWidget = parentWidget;
	d->passwordsAlreadySet = false;
	d->server = server;
}

JoinCommandLineBuilder::~JoinCommandLineBuilder()
{
}

void JoinCommandLineBuilder::allDownloadableWads(const JoinError &joinError, QStringList &required, QStringList &optional)
{
	if (!joinError.missingIwad().isEmpty())
	{
		required << joinError.missingIwad();
	}

	const QList<PWad> missingWads = joinError.missingWads();
	foreach(const PWad &wad, missingWads)
	{
		if(wad.isOptional())
			optional.append(wad.name());
		else
			required.append(wad.name());
	}
	required = Wadseeker::filterAllowedOnlyWads(required);
	optional = Wadseeker::filterAllowedOnlyWads(optional);
}

bool JoinCommandLineBuilder::buildServerConnectParams(ServerConnectParams &params)
{
	if (d->server->isLockedAnywhere())
	{
		if (!d->passwordsAlreadySet)
		{
			PasswordDlg password(d->server);
			int ret = password.exec();
			if (ret != QDialog::Accepted)
			{
				return false;
			}
			d->connectPassword = password.connectPassword();
			d->inGamePassword = password.inGamePassword();
			d->passwordsAlreadySet = true;
		}
		params.setConnectPassword(d->connectPassword);
		params.setInGamePassword(d->inGamePassword);
	}

	if (!d->demoName.isEmpty())
	{
		params.setDemoName(d->demoName);
	}
	return true;
}

const CommandLineInfo &JoinCommandLineBuilder::builtCommandLine() const
{
	return d->cli;
}

bool JoinCommandLineBuilder::checkServerStatus()
{
	// Remember to check REFRESHING status first!
	if (d->server->isRefreshing())
	{
		d->error = tr("This server is still refreshing.\nPlease wait until it is finished.");
		gLog << tr("Attempted to obtain a join command line for a \"%1\" "
			"server that is under refresh.").arg(d->server->addressWithPort());
		return false;
	}
	// Fail if Doomseeker couldn't get data on this server.
	else if (!d->server->isKnown())
	{
		d->error = tr("Data for this server is not available.\nOperation failed.");
		gLog << tr("Attempted to obtain a join command line for an unknown server \"%1\"").arg(
			d->server->addressWithPort());
		return false;
	}
	return true;
}

bool JoinCommandLineBuilder::checkWadseekerValidity(QWidget *parent)
{
	QString targetDirPath = gConfig.wadseeker.targetDirectory;
	QDir targetDir(targetDirPath);
	QFileInfo targetDirFileInfo(targetDirPath);

	if (targetDirPath.isEmpty() || !targetDir.exists() || !targetDirFileInfo.isWritable())
	{
		return false;
	}

	return true;
}

int JoinCommandLineBuilder::displayMissingWadsMessage(const QStringList &downloadableWads,
	QStringList &optionalWads, const QString &message)
{
	const QString CAPTION = tr("Doomseeker - files are missing");

	// Can't use QMessageBox here because we need to be able to add our
	// optional wad selection box.
	QDialog msgBox;
	msgBox.setWindowTitle(CAPTION);
	QGridLayout *grid = new QGridLayout;
	QLabel *mainMessage = new QLabel;
	grid->addWidget(mainMessage, 0, 1);

	QListWidget *optionalList = NULL;
	if(!optionalWads.isEmpty())
	{
		optionalList = new QListWidget;
		optionalList->setMaximumHeight(64);
		grid->addWidget(optionalList, 1, 1);
		foreach(const QString &wad, optionalWads)
		{
			QListWidgetItem *item = new QListWidgetItem(wad, optionalList);
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
			item->setCheckState(Qt::Checked);
		}
	}

	QLabel *questionLabel = new QLabel;
	grid->addWidget(questionLabel, 2, 1);

	// We'll need to store this in our d-pointer and connect a signal to this
	// object since a button box doesn't give us an easy way to get the button
	// clicked to close the dialog.
	d->buttonBox = new QDialogButtonBox;
	d->lastButtonClicked = QDialogButtonBox::NoButton;
	grid->addWidget(d->buttonBox, 3, 0, 1, 2, Qt::AlignRight);
	msgBox.connect(d->buttonBox, SIGNAL(accepted()), SLOT(accept()));
	msgBox.connect(d->buttonBox, SIGNAL(rejected()), SLOT(reject()));
	connect(d->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(missingWadsClicked(QAbstractButton*)));

	QLabel *icon = new QLabel;
	QIcon questionIcon = msgBox.style()->standardIcon(QStyle::SP_MessageBoxQuestion);
	icon->setPixmap(questionIcon.pixmap(questionIcon.actualSize(QSize(64,64))));
	grid->addWidget(icon, 0, 0, 3, 1, Qt::AlignTop);
	
	msgBox.setLayout(grid);

	QString ignoreMessage;
	if (d->server->plugin()->data()->inGameFileDownloads)
	{
		ignoreMessage = tr("Alternatively use ignore to connect anyways.");
		d->buttonBox->addButton(QDialogButtonBox::Ignore);
	}

	QString questionMessage;
	if (!downloadableWads.isEmpty() || !optionalWads.isEmpty())
	{
		questionLabel->setText(QString("%1\n%2").arg(tr("Do you want Wadseeker to find the missing WADs?")).arg(ignoreMessage));
		d->buttonBox->addButton(QDialogButtonBox::Yes);
		d->buttonBox->addButton(QDialogButtonBox::No);
		mainMessage->setText(QString("%1\n\n%2").arg(message).arg(tr("Following files can be downloaded: %1").arg(downloadableWads.join(", "))));
		
	}
	else
	{
		questionLabel->setText(ignoreMessage);
		d->buttonBox->addButton(QDialogButtonBox::Ok);
		mainMessage->setText(message);
	}

	msgBox.exec();

	// Clear out unselected optional wads.
	for(int i = optionalWads.size();i-- > 0;)
	{
		if(optionalList->item(i)->checkState() != Qt::Checked)
			optionalWads.removeAt(i);
	}

	return d->lastButtonClicked;
}

const QString &JoinCommandLineBuilder::error() const
{
	return d->error;
}

void JoinCommandLineBuilder::failBuild()
{
	d->cli = CommandLineInfo();
	emit commandLineBuildFinished();
}

void JoinCommandLineBuilder::handleError(const JoinError &error)
{
	if (!error.error().isEmpty())
	{
		d->error = error.error();
	}
	else
	{
		d->error = tr("Unknown error.");
	}
	d->configurationError = (error.type() == JoinError::ConfigurationError);

	gLog << tr("Error when obtaining join parameters for server "
		"\"%1\", game \"%2\": %3").arg(d->server->name()).arg(
			d->server->engineName()).arg(d->error);
}

JoinCommandLineBuilder::MissingWadsProceed JoinCommandLineBuilder::handleMissingWads(const JoinError &error)
{
	if (WadseekerInterface::isInstantiated())
	{
		QMessageBox::StandardButtons ret =
			QMessageBox::warning(d->parentWidget, tr("Doomseeker - files are missing"),
				tr("You don't have all the files required by this server and an instance "
					"of Wadseeker is already running.\n\n"
					"Press 'Ignore' to join anyway."),
				QMessageBox::Abort | QMessageBox::Ignore);
		return ret == QMessageBox::Ignore ? Ignore : Cancel;;
	}

	QString filesMissingMessage = tr("Following files are missing:\n");

	if (!error.missingIwad().isEmpty())
	{
		filesMissingMessage += tr("IWAD: ") + error.missingIwad().toLower() + "\n";
		if (Wadseeker::isForbiddenWad(error.missingIwad()))
		{
			filesMissingMessage += tr("\n"
				"Make sure that this file is in one of the paths "
				"specified in Options -> File Paths.\n"
				"This file belongs to a commercial game or is otherwise "
				"blocked from download. If you don't have this file, "
				"and it belongs to a commercial game, "
				"you need to purchase the game associated with this IWAD.\n"
				"Wadseeker will not download commercial IWADs.\n\n");
		}
	}

	if (!error.missingWads().isEmpty())
	{
		const QList<PWad> missingWads = error.missingWads();
		QStringList wadlist;
		QStringList optionals;
		foreach(const PWad &wad, missingWads)
		{
			if(wad.isOptional())
				optionals << wad.name();
			else
				wadlist << wad.name();
		}
		filesMissingMessage += tr("PWADS: %1\nOptional PWADs: %2").arg(wadlist.join(", ")).arg(optionals.join(", "));
	}

	QStringList requiredDownloads, optionalDownloads;
	allDownloadableWads(error, requiredDownloads, optionalDownloads);
	QMessageBox::StandardButtons ret = (QMessageBox::StandardButtons)
		displayMissingWadsMessage(requiredDownloads, optionalDownloads, filesMissingMessage);
	if (ret == QMessageBox::Yes)
	{
		if (!gWadseekerShow->checkWadseekerValidity(d->parentWidget))
		{
			return Cancel;
		}

		WadseekerInterface *wadseeker = WadseekerInterface::create(d->server);
		this->connect(wadseeker, SIGNAL(finished(int)), SLOT(onWadseekerDone(int)));
		requiredDownloads.append(optionalDownloads); // Pass in all requested downloads to Wadseeker
		wadseeker->setWads(requiredDownloads);
		wadseeker->setAttribute(Qt::WA_DeleteOnClose);
		wadseeker->show();
		return Seeking;
	}
	return ret == QMessageBox::Ignore ? Ignore : Cancel;
}

bool JoinCommandLineBuilder::isConfigurationError() const
{
	return d->configurationError;
}

void JoinCommandLineBuilder::missingWadsClicked(QAbstractButton *button)
{
	d->lastButtonClicked = d->buttonBox->standardButton(button);
}

void JoinCommandLineBuilder::obtainJoinCommandLine()
{
	assert(d->server != NULL);
	d->cli = CommandLineInfo();

	if (!checkServerStatus())
	{
		failBuild();
		return;
	}

	ServerConnectParams params;
	if (!buildServerConnectParams(params))
	{
		failBuild();
		return;
	}
	GameClientRunner* gameRunner = d->server->gameRunner();
	JoinError joinError = gameRunner->createJoinCommandLine(d->cli, params);
	delete gameRunner;

	switch (joinError.type())
	{
		case JoinError::Terminate:
			failBuild();
			return;
		case JoinError::ConfigurationError:
		case JoinError::Critical:
		{
			handleError(joinError);
			failBuild();
			return;
		}

		case JoinError::CanAutomaticallyInstallGame:
		{
			if (tryToInstallGame())
			{
				obtainJoinCommandLine();
			}
			else
			{
				failBuild();
			}
			return;
		}

		case JoinError::MissingWads:
		{
			MissingWadsProceed proceed = handleMissingWads(joinError);
			switch (proceed)
			{
				case Cancel:
					failBuild();
					return;
				case Ignore:
					break;
				case Seeking:
					// async process; will call slot
					return;
				default:
					gLog << "Bug: not sure how to proceed after \"MissingWads\".";
					failBuild();
					return;
			}
			// Intentional fall through
		}

		case JoinError::NoError:
			if (d->demo == GameDemo::Managed)
			{
				QStringList pwads;
				foreach (const PWad &wad, d->server->wads())
				{
					pwads << wad.name();
				}
				GameDemo::saveDemoMetaData(d->demoName, *d->server->plugin(),
					d->server->iwad(), pwads);
			}
			break;

		default:
			gLog << "JoinCommandLineBuilder - unhandled JoinError type!";
			break;
	}

	emit commandLineBuildFinished();
}

void JoinCommandLineBuilder::onWadseekerDone(int result)
{
	qDebug() << "onWadseekerDone:" << result;
	if (result == QDialog::Accepted)
	{
		obtainJoinCommandLine();
	}
}

ServerPtr JoinCommandLineBuilder::server() const
{
	return d->server;
}

bool JoinCommandLineBuilder::tryToInstallGame()
{
	Message message = d->server->clientExe()->install(gApp->mainWindowAsQWidget());
	if (message.isError())
	{
		QMessageBox::critical(gApp->mainWindowAsQWidget(), tr("Game installation failure"),
			message.contents(), QMessageBox::Ok);
	}
	return message.type() == Message::Type::SUCCESSFUL;
}
