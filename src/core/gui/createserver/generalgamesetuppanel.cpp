//------------------------------------------------------------------------------
// generalgamesetuppanel.cpp
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
#include "generalgamesetuppanel.h"

#include "configuration/doomseekerconfig.h"
#include "ini/ini.h"
#include "plugins/engineplugin.h"
#include "serverapi/exefile.h"
#include "serverapi/gamecreateparams.h"
#include "serverapi/gameexeretriever.h"
#include "serverapi/message.h"
#include "serverapi/serverstructs.h"
#include "serverapi/server.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QHostAddress>
#include <QMessageBox>

class GeneralGameSetupPanel::PrivData
{
public:
	EnginePlugin *currentEngine;
	bool suppressMissingExeErrors;
	bool remoteGameSetup;
};


GeneralGameSetupPanel::GeneralGameSetupPanel(QWidget *parent)
: QWidget(parent)
{
	setupUi(this);
	d = new PrivData();
	d->remoteGameSetup = false;
	d->suppressMissingExeErrors = false;

	this->connect(cboEngine, SIGNAL(currentPluginChanged(EnginePlugin*)),
		SIGNAL(pluginChanged(EnginePlugin*)));
}

GeneralGameSetupPanel::~GeneralGameSetupPanel()
{
	delete d;
}

void GeneralGameSetupPanel::fillInParams(GameCreateParams &params, bool offline)
{
	params.setExecutablePath(pathToExe(offline));
	params.setHostMode(offline ? GameCreateParams::Offline : GameCreateParams::Host);
	params.setIwadPath(iwadPicker->currentIwad());
	params.setPwadsPaths(wadsPicker->filePaths());
	params.setBroadcastToLan(cbBroadcastToLAN->isChecked());
	params.setBroadcastToMaster(cbBroadcastToMaster->isChecked());
	params.setMap(leMap->text());
	params.setName(leServername->text());
	params.setPort(spinPort->isEnabled() ? spinPort->value() : 0);
	params.setGameMode(currentGameMode());
}

void GeneralGameSetupPanel::loadConfig(Ini &config)
{
	IniSection general = config.section("General");

	// General
	if (!d->remoteGameSetup)
	{
		QString engineName = general["engine"];
		const EnginePlugin* prevEngine = d->currentEngine;
		if(!setEngine(engineName))
			return;

		bool bChangeExecutable = (prevEngine != d->currentEngine || !cbLockExecutable->isChecked());

		// First let's check if we can use executable stored in the server's config.
		// We will save the path to this executable in a local variable.
		QString executablePath = "";
		if (bChangeExecutable)
		{
			executablePath = *general["executable"];
			QFileInfo fileInfo(executablePath);
			if (!fileInfo.exists())
			{
				// Executable cannot be found, display error message and reset
				// the local variable.
				QMessageBox::warning(this, tr("Doomseeker - load server config"),
					tr("Game executable saved in config cannot be found.\n"
						"Default executable will be used."));
				Message message;
				executablePath = pathToServerExe(message);
			}
		}

		// If we successfuly retrieved path from the config we shall
		// set this path in the line edit control.
		if (!executablePath.isEmpty())
		{
			leExecutable->setText(executablePath);
		}
	}

	leServername->setText(general["name"]);
	spinPort->setValue(general["port"]);
	cboGamemode->setCurrentIndex(general["gamemode"]);
	leMap->setText(general["map"]);
	iwadPicker->addIwad(general["iwad"]);

	wadsPicker->setFilePaths(general["pwads"].valueString().split(";"));

	cbBroadcastToLAN->setChecked(general["broadcastToLAN"]);
	cbBroadcastToMaster->setChecked(general["broadcastToMaster"]);
}

void GeneralGameSetupPanel::saveConfig(Ini &config)
{
	IniSection general = config.section("General");
	general["engine"] = cboEngine->currentText();
	general["executable"] = leExecutable->text();
	general["name"] = leServername->text();
	general["port"] = spinPort->value();
	general["gamemode"] = cboGamemode->currentIndex();
	general["map"] = leMap->text();
	general["iwad"] = iwadPicker->currentIwad();

	general["pwads"] = wadsPicker->filePaths().join(";");

	general["broadcastToLAN"] = cbBroadcastToLAN->isChecked();
	general["broadcastToMaster"] = cbBroadcastToMaster->isChecked();
}

void GeneralGameSetupPanel::setupForEngine(EnginePlugin *engine)
{
	d->currentEngine = engine;
	// Executable path
	Message message;

	if (d->remoteGameSetup)
	{
		// When we setup a remote game, we want to use a client
		// executable to connect to it.
		ServerPtr server = engine->server(QHostAddress("127.0.0.1"), 1);
		leExecutable->setText(pathToClientExe(server.data(), message));
	}
	else
	{
		leExecutable->setText(pathToServerExe(message));
	}

	if (message.isError() && !d->suppressMissingExeErrors)
	{
		QString caption = tr("Doomseeker - error obtaining server binary");
		QString error = tr("Server binary for engine \"%1\" cannot be obtained.\n"
			"Following error has occured:\n%2")
			.arg(d->currentEngine->data()->name, message.contents());

		QMessageBox::warning(this, caption, error);
	}

	spinPort->setValue(d->currentEngine->data()->defaultServerPort);

	cboGamemode->clear();
	const QList<GameMode> &gameModes = d->currentEngine->data()->gameModes;
	if (!gameModes.isEmpty())
	{
		for (int i = 0; i < gameModes.count(); ++i)
		{
			cboGamemode->addItem(gameModes[i].name(), i);
		}
	}
}

void GeneralGameSetupPanel::setupForRemoteGame()
{
	d->suppressMissingExeErrors = true;
	d->remoteGameSetup = true;
	cbAllowTheGameToChoosePort->hide();
	QWidget *disableControls[] =
	{
		cboEngine, leExecutable, btnBrowseExecutable, btnDefaultExecutable,
		cbLockExecutable, leServername, spinPort, cbBroadcastToLAN,
		cbBroadcastToMaster,

		NULL
	};
	for(int i = 0;disableControls[i] != NULL;++i)
		disableControls[i]->setDisabled(true);
}

QString GeneralGameSetupPanel::pathToExe(bool offline)
{
	// Since some operating systems have different offline and server binaries
	// We will see if they are playing offline and switch to the client
	// binary if the specified executable is the same as what is provided
	// as the server.
	Message message;
	QString offlineExePath = pathToOfflineExe(message);
	QString serverExePath = pathToServerExe(message);
	bool bIsLineEditPotiningToServerBinary = (leExecutable->text() == serverExePath);
	bool bShouldUseClientBinary = (offline || d->remoteGameSetup) && message.isIgnore() && bIsLineEditPotiningToServerBinary;

	if (bShouldUseClientBinary)
	{
		return offlineExePath;
	}
	else
	{
		return leExecutable->text();
	}
}

QString GeneralGameSetupPanel::pathToClientExe(Server* server, Message& message)
{
	QScopedPointer<ExeFile> f(server->clientExe());
	return f->pathToExe(message);
}

QString GeneralGameSetupPanel::pathToOfflineExe(Message& message)
{
	return GameExeRetriever(*d->currentEngine->gameExe()).pathToOfflineExe(message);
}

QString GeneralGameSetupPanel::pathToServerExe(Message& message)
{
	return GameExeRetriever(*d->currentEngine->gameExe()).pathToServerExe(message);
}

void GeneralGameSetupPanel::browseExecutable()
{
	QString dialogDir = gConfig.doomseeker.previousCreateServerExecDir;
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - Add file"), dialogDir);

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		gConfig.doomseeker.previousCreateServerExecDir = fi.absolutePath();

		leExecutable->setText(fi.absoluteFilePath());
	}
}

void GeneralGameSetupPanel::setExecutableToDefault()
{
	Message message;
	leExecutable->setText(pathToServerExe(message));

	if (!message.isIgnore())
	{
		QMessageBox::critical(this, tr("Obtaining default server binary path."),
			message.contents(),QMessageBox::Ok, QMessageBox::Ok);
	}
}

void GeneralGameSetupPanel::onGameModeChanged(int index)
{
	if (index >= 0)
	{
		const QList<GameMode> &gameModes = d->currentEngine->data()->gameModes;
		emit gameModeChanged(gameModes[index]);
	}
}

GameMode GeneralGameSetupPanel::currentGameMode() const
{
	const QList<GameMode> &gameModes = d->currentEngine->data()->gameModes;
	foreach (const GameMode& mode, gameModes)
	{
		if (mode.name().compare(cboGamemode->currentText()) == 0)
		{
			return mode;
		}
	}
	return GameMode();
}

EnginePlugin *GeneralGameSetupPanel::currentPlugin() const
{
	return cboEngine->currentPlugin();
}

bool GeneralGameSetupPanel::setEngine(const QString &engineName)
{
	if (!cboEngine->setPluginByName(engineName))
	{
		QMessageBox::critical(this, tr("Doomseeker - load server config"),
			tr("Plugin for engine \"%1\" is not present!").arg(engineName));
		return false;
	}
	return true;
}
