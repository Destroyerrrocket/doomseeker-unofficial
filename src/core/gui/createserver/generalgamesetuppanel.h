//------------------------------------------------------------------------------
// generalgamesetuppanel.h
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
#ifndef idb76a672e_9f54_4dbc_a933_33ea083e8f11
#define idb76a672e_9f54_4dbc_a933_33ea083e8f11

#include "dptr.h"

#include <QWidget>

class CreateServerDialog;
class EnginePlugin;
class GameCreateParams;
class GameMode;
class Ini;
class Message;
class Server;

class GeneralGameSetupPanel : public QWidget
{
Q_OBJECT

public:
	GeneralGameSetupPanel(QWidget *parent);
	~GeneralGameSetupPanel();

	GameMode currentGameMode() const;
	EnginePlugin *currentPlugin() const;
	void fillInParams(GameCreateParams &params, bool offline);
	void loadConfig(Ini &config, bool loadingPrevious);
	QString mapName() const;
	void saveConfig(Ini &config);
	void setupForEngine(EnginePlugin *engine);
	void setupForRemoteGame();
	void setCreateServerDialog(CreateServerDialog *dialog);
	void setIwadByName(const QString &iwad);

signals:
	void gameModeChanged(const GameMode &gameMode);
	void pluginChanged(EnginePlugin *plugin);

protected:
	void showEvent(QShowEvent *event);

private:
	DPtr<GeneralGameSetupPanel> d;

	QString pathToExe(bool offline);
	QString pathToClientExe(Server* server, Message& message);
	QString pathToOfflineExe(Message& message);
	QString pathToServerExe(Message& message);

	bool setEngine(const QString &engineName);

private slots:
	void browseExecutable();
	void onGameModeChanged(int);
	void setExecutableToDefault();
	void updateMapWarningVisibility();
};

#endif
