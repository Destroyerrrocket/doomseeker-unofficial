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
#include "ui_generalgamesetuppanel.h"

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

DClass<GeneralGameSetupPanel> : public Ui::GeneralGameSetupPanel
{
public:
	EnginePlugin *currentEngine;
	bool iwadSetExplicitly;
	bool suppressMissingExeErrors;
	bool remoteGameSetup;
};

DPointered(GeneralGameSetupPanel)


GeneralGameSetupPanel::GeneralGameSetupPanel(QWidget *parent)
: QWidget(parent)
{
	d->setupUi(this);
	d->iwadSetExplicitly = false;
	d->remoteGameSetup = false;
	d->suppressMissingExeErrors = false;

	this->connect(d->cboEngine, SIGNAL(currentPluginChanged(EnginePlugin*)),
		SIGNAL(pluginChanged(EnginePlugin*)));
}

GeneralGameSetupPanel::~GeneralGameSetupPanel()
{
}

void GeneralGameSetupPanel::fillInParams(GameCreateParams &params, bool offline)
{
	params.setExecutablePath(pathToExe(offline));
	params.setHostMode(offline ? GameCreateParams::Offline : GameCreateParams::Host);
	params.setIwadPath(d->iwadPicker->currentIwad());
	params.setPwadsPaths(d->wadsPicker->filePaths());
	params.setPwadsOptional(d->wadsPicker->fileOptional());
	params.setBroadcastToLan(d->cbBroadcastToLAN->isChecked());
	params.setBroadcastToMaster(d->cbBroadcastToMaster->isChecked());
	params.setMap(d->leMap->text());
	params.setName(d->leServername->text());
	params.setPort(d->spinPort->isEnabled() ? d->spinPort->value() : 0);
	params.setGameMode(currentGameMode());
}

void GeneralGameSetupPanel::loadConfig(Ini &config, bool loadingPrevious)
{
	IniSection general = config.section("General");

	// General
	if (!d->remoteGameSetup)
	{
		QString engineName = general["engine"];
		const EnginePlugin* prevEngine = d->currentEngine;
		if(!setEngine(engineName))
			return;

		bool bChangeExecutable = (prevEngine != d->currentEngine || !d->cbLockExecutable->isChecked());

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
			d->leExecutable->setText(executablePath);
		}
	}

	d->leServername->setText(general["name"]);
	d->spinPort->setValue(general["port"]);
	d->cboGamemode->setCurrentIndex(general["gamemode"]);
	d->leMap->setText(general["map"]);
	
	if (!(loadingPrevious && d->iwadSetExplicitly))
	{
		d->iwadPicker->addIwad(general["iwad"]);
	}

	QList<bool> optionalWads;
	foreach(QString value, general["pwadsOptional"].valueString().split(";"))
	{
		optionalWads << (value != "0");
	}	
	d->wadsPicker->setFilePaths(general["pwads"].valueString().split(";"), optionalWads);

	d->cbBroadcastToLAN->setChecked(general["broadcastToLAN"]);
	d->cbBroadcastToMaster->setChecked(general["broadcastToMaster"]);
}

void GeneralGameSetupPanel::saveConfig(Ini &config)
{
	IniSection general = config.section("General");
	general["engine"] = d->cboEngine->currentText();
	general["executable"] = d->leExecutable->text();
	general["name"] = d->leServername->text();
	general["port"] = d->spinPort->value();
	general["gamemode"] = d->cboGamemode->currentIndex();
	general["map"] = d->leMap->text();
	general["iwad"] = d->iwadPicker->currentIwad();

	general["pwads"] = d->wadsPicker->filePaths().join(";");
	QList<bool> optionalWads = d->wadsPicker->fileOptional();
	QStringList optionalList;
	foreach(bool optional, optionalWads)
		optionalList << (optional ? "1" : "0");
	general["pwadsOptional"] = optionalList.join(";");

	general["broadcastToLAN"] = d->cbBroadcastToLAN->isChecked();
	general["broadcastToMaster"] = d->cbBroadcastToMaster->isChecked();
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
		d->leExecutable->setText(pathToClientExe(server.data(), message));
	}
	else
	{
		d->leExecutable->setText(pathToServerExe(message));
	}

	if (message.isError() && !d->suppressMissingExeErrors)
	{
		QString caption = tr("Doomseeker - error obtaining server binary");
		QString error = tr("Server binary for engine \"%1\" cannot be obtained.\n"
			"Following error has occured:\n%2")
			.arg(d->currentEngine->data()->name, message.contents());

		QMessageBox::warning(this, caption, error);
	}

	d->spinPort->setValue(d->currentEngine->data()->defaultServerPort);

	d->cboGamemode->clear();
	QList<GameMode> gameModes = d->currentEngine->gameModes();
	if (!gameModes.isEmpty())
	{
		for (int i = 0; i < gameModes.count(); ++i)
		{
			d->cboGamemode->addItem(gameModes[i].name(), i);
		}
	}
}

void GeneralGameSetupPanel::setupForRemoteGame()
{
	d->suppressMissingExeErrors = true;
	d->remoteGameSetup = true;
	d->cbAllowTheGameToChoosePort->hide();
	QWidget *disableControls[] =
	{
		d->cboEngine, d->leExecutable, d->btnBrowseExecutable, d->btnDefaultExecutable,
		d->cbLockExecutable, d->leServername, d->spinPort, d->cbBroadcastToLAN,
		d->cbBroadcastToMaster,

		NULL
	};
	for(int i = 0;disableControls[i] != NULL;++i)
		disableControls[i]->setDisabled(true);
}

void GeneralGameSetupPanel::setIwadByName(const QString &iwad)
{
	d->iwadSetExplicitly = true;
	d->iwadPicker->setIwadByName(iwad);
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
	bool bIsLineEditPotiningToServerBinary = (d->leExecutable->text() == serverExePath);
	bool bShouldUseClientBinary = (offline || d->remoteGameSetup) && message.isIgnore() && bIsLineEditPotiningToServerBinary;

	if (bShouldUseClientBinary)
	{
		return offlineExePath;
	}
	else
	{
		return d->leExecutable->text();
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

		d->leExecutable->setText(fi.absoluteFilePath());
	}
}

void GeneralGameSetupPanel::setExecutableToDefault()
{
	Message message;
	d->leExecutable->setText(pathToServerExe(message));

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
		QList<GameMode> gameModes = d->currentEngine->gameModes();
		emit gameModeChanged(gameModes[index]);
	}
}

GameMode GeneralGameSetupPanel::currentGameMode() const
{
	QList<GameMode> gameModes = d->currentEngine->gameModes();
	foreach (const GameMode& mode, gameModes)
	{
		if (mode.name().compare(d->cboGamemode->currentText()) == 0)
		{
			return mode;
		}
	}
	return GameMode();
}

EnginePlugin *GeneralGameSetupPanel::currentPlugin() const
{
	return d->cboEngine->currentPlugin();
}

bool GeneralGameSetupPanel::setEngine(const QString &engineName)
{
	if (!d->cboEngine->setPluginByName(engineName))
	{
		QMessageBox::critical(this, tr("Doomseeker - load server config"),
			tr("Plugin for engine \"%1\" is not present!").arg(engineName));
		return false;
	}
	return true;
}
