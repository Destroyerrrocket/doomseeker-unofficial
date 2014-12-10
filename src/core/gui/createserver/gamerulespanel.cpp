//------------------------------------------------------------------------------
// gamerulespanel.cpp
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
#include "gamerulespanel.h"

#include "ini/ini.h"
#include "plugins/engineplugin.h"
#include "serverapi/gamecreateparams.h"
#include "serverapi/serverstructs.h"

class GameRulesPanel::PrivData
{
public:
	class GameLimitWidget
	{
		public:
			QWidget* label;
			QSpinBox* spinBox;
			GameCVar limit;
	};

	QList<GameCVar> gameModifiers;
	QList<GameLimitWidget*> limitWidgets;
};

GameRulesPanel::GameRulesPanel(QWidget *parent)
: QWidget(parent)
{
	d = new PrivData();
	setupUi(this);
}

GameRulesPanel::~GameRulesPanel()
{
	qDeleteAll(d->limitWidgets);
	delete d;
}

void GameRulesPanel::fillInParams(GameCreateParams &params)
{
	params.setSkill(cboDifficulty->currentIndex());
	params.setMaxClients(spinMaxClients->value());
	params.setMaxPlayers(spinMaxPlayers->value());

	fillInLimits(params);
	fillInModifiers(params);

	mapListPanel->fillInParams(params);
}

void GameRulesPanel::fillInLimits(GameCreateParams &params)
{
	foreach(PrivData::GameLimitWidget* p, d->limitWidgets)
	{
		p->limit.setValue(p->spinBox->value());
		params.cvars() << p->limit;
	}
}

void GameRulesPanel::fillInModifiers(GameCreateParams &params)
{
	int modIndex = cboModifier->currentIndex();
	if (modIndex > 0) // Index zero is always "< NONE >"
	{
		--modIndex;
		d->gameModifiers[modIndex].setValue(1);
		params.cvars() << d->gameModifiers[modIndex];
	}
}

void GameRulesPanel::loadConfig(Ini &config)
{
	IniSection section = config.section("Rules");

	cboDifficulty->setCurrentIndex(section["difficulty"]);
	cboModifier->setCurrentIndex(section["modifier"]);
	spinMaxClients->setValue(section["maxClients"]);
	spinMaxPlayers->setValue(section["maxPlayers"]);
	foreach (PrivData::GameLimitWidget* widget, d->limitWidgets)
	{
		widget->spinBox->setValue(section[widget->limit.command()]);
	}

	mapListPanel->loadConfig(config);
}

void GameRulesPanel::saveConfig(Ini &config)
{
	IniSection section = config.section("Rules");

	section["difficulty"] = cboDifficulty->currentIndex();
	section["modifier"] = cboModifier->currentIndex();
	section["maxClients"] = spinMaxClients->value();
	section["maxPlayers"] = spinMaxPlayers->value();
	foreach (PrivData::GameLimitWidget *widget, d->limitWidgets)
	{
		section[widget->limit.command()] = widget->spinBox->value();
	}

	mapListPanel->saveConfig(config);
}

void GameRulesPanel::setupForEngine(const EnginePlugin *engine, const GameMode &gameMode)
{
	setupDifficulty();
	setupModifiers(engine);
	mapListPanel->setupForEngine(engine);

	setupLimitWidgets(engine, gameMode);
}

void GameRulesPanel::setupForRemoteGame()
{
	QWidget *disableControls[] =
	{
		spinMaxClients, spinMaxPlayers, NULL
	};
	for (int i = 0; disableControls[i]; ++i)
		disableControls[i]->setDisabled(true);
}

void GameRulesPanel::setupDifficulty()
{
	cboDifficulty->clear();

	cboDifficulty->addItem("1 - I'm too young to die", 0);
	cboDifficulty->addItem("2 - Hey, not too rough", 1);
	cboDifficulty->addItem("3 - Hurt me plenty", 2);
	cboDifficulty->addItem("4 - Ultra-violence", 3);
	cboDifficulty->addItem("5 - NIGHTMARE!", 4);
}

void GameRulesPanel::setupModifiers(const EnginePlugin *engine)
{
	cboModifier->clear();
	d->gameModifiers.clear();

	const QList<GameCVar> &modifiers = engine->data()->gameModifiers;

	if (!modifiers.isEmpty())
	{
		cboModifier->show();
		labelModifiers->show();

		cboModifier->addItem(tr("< NONE >"));

		foreach (const GameCVar &cvar, modifiers)
		{
			cboModifier->addItem(cvar.name());
			d->gameModifiers << cvar;
		}
	}
	else
	{
		cboModifier->hide();
		labelModifiers->hide();
	}
}

void GameRulesPanel::removeLimitWidgets()
{
	foreach (PrivData::GameLimitWidget *widget, d->limitWidgets)
	{
		delete widget->label;
		delete widget->spinBox;
		delete widget;
	}

	d->limitWidgets.clear();
}

void GameRulesPanel::setupLimitWidgets(const EnginePlugin *engine, const GameMode &gameMode)
{
	removeLimitWidgets();
	QList<GameCVar> limits = engine->limits(gameMode);
	QList<GameCVar>::iterator it;

	int number = 0;
	for (it = limits.begin(); it != limits.end(); ++it, ++number)
	{
		QLabel* label = new QLabel(this);
		label->setText(it->name());
		QSpinBox* spinBox = new QSpinBox(this);
		spinBox->setMaximum(999999);

		limitsLayout->addRow(label, spinBox);

		PrivData::GameLimitWidget* glw = new PrivData::GameLimitWidget();
		glw->label = label;
		glw->spinBox = spinBox;
		glw->limit = (*it);
		d->limitWidgets << glw;
	}
}
