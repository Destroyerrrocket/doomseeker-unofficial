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
#include "gui/passwordDlg.h"
#include "gui/wadseekerinterface.h"
#include "gui/wadseekershow.h"
#include "ini/settingsproviderqt.h"
#include "plugins/engineplugin.h"
#include "serverapi/exefile.h"
#include "serverapi/gameclientrunner.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "application.h"

#include <wadseeker/wadseeker.h>
#include <QMessageBox>
#include <cassert>

class JoinCommandLineBuilder::PrivData
{
	public:
		CommandLineInfo cli;
		bool configurationError;
		QString error;
		Demo demo;
		ServerPtr server;
		QWidget *parentWidget;
};

JoinCommandLineBuilder::JoinCommandLineBuilder(ServerPtr server,
	Demo demo, QWidget *parentWidget)
{
	d = new PrivData();
	d->configurationError = false;
	d->demo = demo;
	d->parentWidget = parentWidget;
	d->server = server;
}

JoinCommandLineBuilder::~JoinCommandLineBuilder()
{
	delete d;
}

QStringList JoinCommandLineBuilder::allDownloadableWads(const JoinError &joinError)
{
	QStringList wads;
	if (!joinError.missingIwad().isEmpty())
	{
		wads << joinError.missingIwad();
	}
	wads.append(joinError.missingWads());
	return Wadseeker::filterAllowedOnlyWads(wads);
}

bool JoinCommandLineBuilder::buildServerConnectParams(ServerConnectParams &params)
{
	if(d->server->isLockedAnywhere())
	{
		PasswordDlg password(d->server);
		int ret = password.exec();
		if (ret != QDialog::Accepted)
		{
			return false;
		}
		params.setConnectPassword(password.connectPassword());
		params.setInGamePassword(password.inGamePassword());
	}

	if (gConfig.doomseeker.bRecordDemo)
	{
		params.setDemoName(mkDemoName());
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
	const QString &message)
{
	QString fullMessage = message;
	QString ignoreMessage;
	QMessageBox::StandardButtons buttons = 0;
	if (d->server->plugin()->data()->inGameFileDownloads)
	{
		ignoreMessage = tr("\nAlternatively use ignore to connect anyways.");
		buttons |= QMessageBox::Ignore;
	}

	const QString CAPTION = tr("Doomseeker - files are missing");
	if (!downloadableWads.isEmpty())
	{
		fullMessage += tr("\n\nFollowing files can be downloaded: %1\n\n"
			"Do you want Wadseeker to find the missing WADs?").arg(downloadableWads.join(", "));
		fullMessage += ignoreMessage;
		buttons |= QMessageBox::Yes | QMessageBox::No;
		return QMessageBox::question(d->parentWidget, CAPTION, fullMessage, buttons);
	}
	else
	{
		fullMessage += ignoreMessage;
		buttons |= QMessageBox::Ok;
		return QMessageBox::critical(d->parentWidget, CAPTION, fullMessage, buttons);
	}
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
		filesMissingMessage += tr("PWADS: %1").arg(error.missingWads().join(", "));
	}

	QStringList downloadableWads = allDownloadableWads(error);
	QMessageBox::StandardButtons ret = (QMessageBox::StandardButtons)
		displayMissingWadsMessage(downloadableWads, filesMissingMessage);
	if (ret == QMessageBox::Yes)
	{
		if (!gWadseekerShow->checkWadseekerValidity(d->parentWidget))
		{
			return Cancel;
		}

		WadseekerInterface *wadseeker = WadseekerInterface::create(d->server);
		this->connect(wadseeker, SIGNAL(finished(int)), SLOT(onWadseekerDone(int)));
		wadseeker->setWads(downloadableWads);
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

QString JoinCommandLineBuilder::mkDemoName()
{
	// port-iwad-date-wad
	QString demoName;
	if (d->demo == Managed)
	{
		demoName = gDefaultDataPaths->demosDirectoryPath() + QDir::separator();
	}
	demoName += QString("%1_%2").
		arg(d->server->engineName()).
		arg(QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss"));
	if (!d->server->plugin()->data()->demoExtensionAutomatic)
	{
		demoName += QString(".%1").arg(d->server->plugin()->data()->demoExtension);
	}
	return demoName;
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
			break;
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
			if (d->demo == Managed && gConfig.doomseeker.bRecordDemo)
			{
				saveDemoMetaData(mkDemoName());
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

void JoinCommandLineBuilder::saveDemoMetaData(const QString& demoName)
{
	QString metaFileName;
	// If the extension is automatic we need to add it here
	if(d->server->plugin()->data()->demoExtensionAutomatic)
	{
		metaFileName = QString("%1.%2.ini").arg(demoName)
			.arg(d->server->plugin()->data()->demoExtension);
	}
	else
	{
		metaFileName = demoName + ".ini";
	}

	QSettings settings(metaFileName, QSettings::IniFormat);
	SettingsProviderQt settingsProvider(&settings);
	Ini metaFile(&settingsProvider);
	IniSection metaSection = metaFile.createSection("meta");

	// Get a list of wads for demo name:
	QStringList wadList;
	for (int i = 0; i < d->server->numWads(); ++i)
	{
		// Also be sure to escape any underscores.
		wadList << d->server->wad(i).name().toLower();
	}

	metaSection.createSetting("iwad", d->server->iwad().toLower());
	metaSection.createSetting("pwads", wadList.join(";"));
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
