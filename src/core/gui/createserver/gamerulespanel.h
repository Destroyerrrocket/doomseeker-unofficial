//------------------------------------------------------------------------------
// gamerulespanel.h
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
#ifndef id880d5e15_1a53_40da_bdb9_11d9b4dc3222
#define id880d5e15_1a53_40da_bdb9_11d9b4dc3222

#include "dptr.h"

#include <QWidget>

class CreateServerDialog;
class EnginePlugin;
class GameCreateParams;
class GameMode;
class Ini;
class MapListPanel;

class GameRulesPanel : public QWidget
{
Q_OBJECT

public:
	GameRulesPanel(QWidget *parent);
	~GameRulesPanel();

	void fillInParams(GameCreateParams &params);
	MapListPanel *mapListPanel();
	void loadConfig(Ini &config);
	void saveConfig(Ini &config);
	void setupForEngine(const EnginePlugin *engine, const GameMode &gameMode);
	void setupForRemoteGame();
	void setCreateServerDialog(CreateServerDialog *dialog);

private:
	DPtr<GameRulesPanel> d;

	void fillInLimits(GameCreateParams &params);
	void fillInModifiers(GameCreateParams &params);
	void memorizeLimits();
	void loadMemorizedLimits(const EnginePlugin *engine);
	void removeLimitWidgets();
	void setupDifficulty(const EnginePlugin *engine);
	void setupLimitWidgets(const EnginePlugin *engine, const GameMode &gameMode);
	void setupModifiers(const EnginePlugin *engine);
};

#endif
