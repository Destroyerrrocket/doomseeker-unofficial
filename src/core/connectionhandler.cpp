//------------------------------------------------------------------------------
// connectionhandler.cpp
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
// Copyright (C) 2012 Braden Obrzut <admin@maniacsvault.net>
//                    "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "apprunner.h"
#include "datapaths.h"
#include "log.h"
#include "strings.h"
#include "connectionhandler.h"
#include "configuration/doomseekerconfig.h"
#include "gui/passwordDlg.h"
#include "gui/wadseekerinterface.h"
#include "gui/configuration/doomseekerconfigurationdialog.h"
#include "ini/settingsproviderqt.h"
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include "refresher/refresher.h"
#include "serverapi/gameclientrunner.h"
#include "serverapi/message.h"
#include "serverapi/server.h"

#include <wadseeker/wadseeker.h>
#include <QMessageBox>

class ConnectionHandler::PrivData
{
	public:
		ServerPtr server;
};

ConnectionHandler::ConnectionHandler(ServerPtr server, QWidget *parent, bool handleResponse) : QObject(parent)
{
	d = new PrivData();
	this->parent = parent;
	this->handleResponse = handleResponse;
	d->server = server;
	connect(d->server.data(), SIGNAL(updated(ServerPtr, int)), this, SLOT(checkResponse(ServerPtr, int)));
}

ConnectionHandler::~ConnectionHandler()
{
	delete d;
}

QStringList ConnectionHandler::allDownloadableWads(const JoinError &joinError)
{
	QStringList wads;
	if (!joinError.missingIwad().isEmpty())
	{
		wads << joinError.missingIwad();
	}
	wads.append(joinError.missingWads());
	return Wadseeker::filterAllowedOnlyWads(wads);
}

void ConnectionHandler::checkResponse(const ServerPtr &server, int response)
{
	if(response != Server::RESPONSE_GOOD)
	{
		if(handleResponse)
		{
			switch(response)
			{
				case Server::RESPONSE_TIMEOUT:
					QMessageBox::critical(parent, tr("Doomseeker - join server"), tr("Connection to server timed out."));
					break;
				default:
					QMessageBox::critical(parent, tr("Doomseeker - join server"), tr("An error occured while trying to connect to server."));
					break;
			}
		}

		finish(response);
		return;
	}

	run();
}

bool ConnectionHandler::checkWadseekerValidity(QWidget *parent)
{
	QString targetDirPath = gConfig.wadseeker.targetDirectory;
	QDir targetDir(targetDirPath);
	QFileInfo targetDirFileInfo(targetDirPath);

	if (targetDirPath.isEmpty() || !targetDir.exists() || !targetDirFileInfo.isWritable())
	{
		QString message = tr("Wadseeker will not work correctly: \n"
			"Target directory is either not set, is invalid or cannot be written to.\n"
			"Please review your Configuration and/or refer to online help available from "
			"the Help menu.");

		QMessageBox::warning(parent, tr("Doomseeker - Wadseeker error"), message);
		return false;
	}

	return true;
}

ConnectionHandler *ConnectionHandler::connectByUrl(const QUrl &url)
{
	gLog << QString("Attempting to connect to server: %1").arg(url.toString());

	// Locate plugin by scheme
	const EnginePlugin *handler = NULL;
	for(unsigned int i = 0;i < gPlugins->numPlugins();++i)
	{
		const EnginePlugin *plugin = gPlugins->plugin(i)->info();
		if(plugin->data()->scheme.compare(url.scheme(), Qt::CaseInsensitive) == 0)
		{
			handler = plugin;
			break;
		}
	}
	if(handler == NULL)
	{
		gLog << "Scheme not recognized starting normally.";
		return NULL;
	}

	unsigned short port = url.port(handler->data()->defaultServerPort);
	QString address;
	// We can get the port through QUrl so we'll just create a temporary variable here.
	unsigned short tmp;
	Strings::translateServerAddress(url.host(), address, tmp, QString("localhost:10666"));

	// Create the server object
	ServerPtr server = handler->server(QHostAddress(address), port);
	ConnectionHandler *connectionHandler = new ConnectionHandler(server, NULL, true);
	gRefresher->registerServer(server.data());

	return connectionHandler;
}

void ConnectionHandler::finish(int response)
{
	d->server->disconnect(this);
	emit finished(response);
}

QString ConnectionHandler::mkDemoName(ServerPtr server, bool managedDemo)
{
	// port-iwad-date-wad
	QString demoName;
	if (managedDemo)
	{
		demoName = gDefaultDataPaths->demosDirectoryPath() + QDir::separator();
	}
	demoName += QString("%1_%2").
		arg(server->engineName()).
		arg(QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss"));
	if (!server->plugin()->data()->demoExtensionAutomatic)
	{
		demoName += QString(".%1").arg(server->plugin()->data()->demoExtension);
	}
	return demoName;
}

bool ConnectionHandler::obtainJoinCommandLine(QWidget *parent, ServerPtr server, CommandLineInfo& cli, const QString& errorCaption, bool managedDemo, bool *hadMissing)
{
	// TODO: This method is a monster and it needs refactoring!

	cli.applicationDir = "";
	cli.args.clear();
	cli.executable = QFileInfo("");

	if (server != NULL)
	{
		// Remember to check REFRESHING status first!
		if (server->isRefreshing())
		{
			QMessageBox::warning(parent, errorCaption, tr("This server is still refreshing.\nPlease wait until it is finished."));
			gLog << tr("Attempted to obtain a join command line for a \"%1\" server that is under refresh.").arg(server->addressWithPort());
			return false;
		}
		// Fail if Doomseeker couldn't get data on this server.
		else if (!server->isKnown())
		{
			QMessageBox::critical(parent, errorCaption, tr("Data for this server is not available.\nOperation failed."));
			gLog << tr("Attempted to obtain a join command line for an unknown server \"%1\"").arg(server->addressWithPort());
			return false;
		}

		// For MissingWads:
		const QString filesMissingCaption = tr("Doomseeker - files are missing");
		QString filesMissingMessage = tr("Following files are missing:\n");

		ServerConnectParams params;
		if(server->isLockedAnywhere())
		{
			PasswordDlg password(server);
			int ret = password.exec();
			if(ret != QDialog::Accepted)
			{
				return false;
			}
			params.setConnectPassword(password.connectPassword());
			params.setInGamePassword(password.inGamePassword());
		}

		GameClientRunner* gameRunner = server->gameRunner();
		if (gConfig.doomseeker.bRecordDemo)
		{
			params.setDemoName(mkDemoName(server, managedDemo));
		}
		JoinError joinError = gameRunner->createJoinCommandLine(cli, params);
		delete gameRunner;

		switch (joinError.type())
		{
			case JoinError::Terminate:
				return false;
			case JoinError::ConfigurationError:
			case JoinError::Critical:
			{
				QString error;
				if (!joinError.error().isEmpty())
				{
					error = joinError.error();
				}
				else
				{
					error = tr("Unknown error.");
				}

				QMessageBox::critical(parent, errorCaption, error);
				gLog << tr("Error when obtaining join parameters for server \"%1\", game \"%2\": %3").arg(server->name()).arg(server->engineName()).arg(error);

				if (joinError.type() == JoinError::ConfigurationError)
				{
					DoomseekerConfigurationDialog::openConfiguration(server->plugin());
				}
				return false;
			}

			case JoinError::MissingWads:
				// Execute Wadseeker
				if (!joinError.missingIwad().isEmpty())
				{
					filesMissingMessage += tr("IWAD: ") + joinError.missingIwad().toLower() + "\n";
					if (Wadseeker::isForbiddenWad(joinError.missingIwad()))
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

				if (!joinError.missingWads().isEmpty())
				{
					filesMissingMessage += tr("PWADS: %1").arg(joinError.missingWads().join(", "));
				}

				{
					QStringList downloadableWads = allDownloadableWads(joinError);
					if (downloadableWads.isEmpty())
					{
						QMessageBox::critical(parent, filesMissingCaption, filesMissingMessage, QMessageBox::Ok);
						return false;
					}
					
					filesMissingMessage += tr("\n\nFollowing files can be downloaded: %1\n\n"
						"Do you want Wadseeker to find the missing WADs?").arg(downloadableWads.join(", "));
					QMessageBox::StandardButtons buttons = QMessageBox::Yes|QMessageBox::No;
					if (server->plugin()->data()->inGameFileDownloads)
					{
						filesMissingMessage += tr("\nAlternatively use ignore to connect anyways.");
						buttons |= QMessageBox::Ignore;
					}

					QMessageBox::StandardButtons ret = QMessageBox::question(parent, filesMissingCaption, filesMissingMessage, buttons);
					if (ret == QMessageBox::Yes)
					{
						if (!checkWadseekerValidity(parent))
						{
							return false;
						}

						WadseekerInterface wsi(parent);
						wsi.setAutomatic(true, downloadableWads);
						wsi.setCustomSite(server->webSite());
						if (wsi.exec() == QDialog::Accepted)
						{
							if(hadMissing)
								*hadMissing = true;
							return obtainJoinCommandLine(parent, server, cli, errorCaption, managedDemo, NULL);
						}
					}
					if (ret != QMessageBox::Ignore)
						return false;
				}
				// Intentional fall through

			case JoinError::NoError:
				if (managedDemo && gConfig.doomseeker.bRecordDemo)
				{
					saveDemoMetaData(server, mkDemoName(server, managedDemo));
				}
				break;

			default:
				break;
		}
	}

	return true;
}

void ConnectionHandler::refreshToJoin()
{
	// If the data we have is old we should refresh first to check if we can
	// still properly join the server.
	if(d->server->isRefreshable() && gConfig.doomseeker.bQueryBeforeLaunch)
	{
		gRefresher->registerServer(d->server.data());
	}
	else
	{
		checkResponse(d->server, Server::RESPONSE_GOOD);
	}
}

void ConnectionHandler::run()
{
	bool hadMissing = false;
	CommandLineInfo cli;
	if (obtainJoinCommandLine(parent, d->server, cli, tr("Doomseeker - join server"), true, &hadMissing))
	{
		if(hadMissing)
		{
			// Downloading wads takes an unknown amount of time, a server may
			// have rotated wads, players could have joined, etc so lets refresh.
			refreshToJoin();
			return;
		}

		Message message = AppRunner::runExecutable(cli);
		if (message.isError())
		{
			gLog << tr("Error while launching executable for server \"%1\", game \"%2\": %3")
				.arg(d->server->name()).arg(d->server->engineName()).arg(message.contents());
			QMessageBox::critical(parent, tr("Doomseeker - launch executable"), message.contents());
		}
	}

	finish(Server::RESPONSE_GOOD);
}

void ConnectionHandler::saveDemoMetaData(ServerPtr server, const QString& demoName)
{
	QString metaFileName;
	// If the extension is automatic we need to add it here
	if(server->plugin()->data()->demoExtensionAutomatic)
	{
		metaFileName = QString("%1.%2.ini").arg(demoName)
			.arg(server->plugin()->data()->demoExtension);
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
	for (int i = 0; i < server->numWads(); ++i)
	{
		// Also be sure to escape any underscores.
		wadList << server->wad(i).name().toLower();
	}

	metaSection.createSetting("iwad", server->iwad().toLower());
	metaSection.createSetting("pwads", wadList.join(";"));
}
