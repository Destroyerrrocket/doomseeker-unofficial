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
#include "ui_gamerulespanel.h"

#include "ini/ini.h"
#include "plugins/engineplugin.h"
#include "serverapi/gamecreateparams.h"
#include "serverapi/serverstructs.h"

DClass<GameRulesPanel> : public Ui::GameRulesPanel
{
public:
	class GameLimitWidget
	{
		public:
			QWidget* label;
			QSpinBox* spinBox;
			GameCVar limit;
	};

	const EnginePlugin *engine;
	QList<GameCVar> gameModifiers;
	QList<GameLimitWidget*> limitWidgets;
	QMap<QString, QMap<QString, int> > memorizedLimits;
	QVariant memorizedDifficulty;
};

DPointered(GameRulesPanel)

GameRulesPanel::GameRulesPanel(QWidget *parent)
: QWidget(parent)
{
	d->setupUi(this);
	d->engine = NULL;
}

GameRulesPanel::~GameRulesPanel()
{
	qDeleteAll(d->limitWidgets);
}

void GameRulesPanel::fillInParams(GameCreateParams &params)
{
	params.setSkill(d->cboDifficulty->currentIndex());
	params.setMaxClients(d->spinMaxClients->value());
	params.setMaxPlayers(d->spinMaxPlayers->value());

	fillInLimits(params);
	fillInModifiers(params);

	d->mapListPanel->fillInParams(params);
}

void GameRulesPanel::fillInLimits(GameCreateParams &params)
{
	foreach(PrivData<GameRulesPanel>::GameLimitWidget* p, d->limitWidgets)
	{
		p->limit.setValue(p->spinBox->value());
		params.cvars() << p->limit;
	}
}

void GameRulesPanel::fillInModifiers(GameCreateParams &params)
{
	int modIndex = d->cboModifier->currentIndex();
	if (modIndex > 0) // Index zero is always "< NONE >"
	{
		--modIndex;
		d->gameModifiers[modIndex].setValue(1);
		params.cvars() << d->gameModifiers[modIndex];
	}
}

MapListPanel *GameRulesPanel::mapListPanel()
{
	return d->mapListPanel;
}

void GameRulesPanel::memorizeLimits()
{
	if (d->engine != NULL)
	{
		if (!d->memorizedLimits.contains(d->engine->nameCanonical()))
		{
			d->memorizedLimits[d->engine->nameCanonical()] = QMap<QString, int>();
		}
		QMap<QString, int> &limits = d->memorizedLimits[d->engine->nameCanonical()];
		foreach (const PrivData<GameRulesPanel>::GameLimitWidget *limitWidget, d->limitWidgets)
		{
			limits[limitWidget->limit.command()] = limitWidget->spinBox->value();
		}
	}
}

void GameRulesPanel::loadMemorizedLimits(const EnginePlugin *engine)
{
	if (d->memorizedLimits.contains(engine->nameCanonical()))
	{
		QMap<QString, int> &limits = d->memorizedLimits[engine->nameCanonical()];
		foreach (const PrivData<GameRulesPanel>::GameLimitWidget *limitWidget, d->limitWidgets)
		{
			if (limits.contains(limitWidget->limit.command()))
			{
				limitWidget->spinBox->setValue(limits[limitWidget->limit.command()]);
			}
		}
	}
}

void GameRulesPanel::loadConfig(Ini &config)
{
	IniSection section = config.section("Rules");

	d->cboDifficulty->setCurrentIndex(section["difficulty"]);
	d->cboModifier->setCurrentIndex(section["modifier"]);
	d->spinMaxClients->setValue(section["maxClients"]);
	d->spinMaxPlayers->setValue(section["maxPlayers"]);
	foreach (PrivData<GameRulesPanel>::GameLimitWidget* widget, d->limitWidgets)
	{
		widget->spinBox->setValue(section[widget->limit.command()]);
	}

	d->mapListPanel->loadConfig(config);
}

void GameRulesPanel::saveConfig(Ini &config)
{
	IniSection section = config.section("Rules");

	section["difficulty"] = d->cboDifficulty->currentIndex();
	section["modifier"] = d->cboModifier->currentIndex();
	section["maxClients"] = d->spinMaxClients->value();
	section["maxPlayers"] = d->spinMaxPlayers->value();
	foreach (PrivData<GameRulesPanel>::GameLimitWidget *widget, d->limitWidgets)
	{
		section[widget->limit.command()] = widget->spinBox->value();
	}

	d->mapListPanel->saveConfig(config);
}

void GameRulesPanel::setCreateServerDialog(CreateServerDialog *dialog)
{
	d->mapListPanel->setCreateServerDialog(dialog);
}

void GameRulesPanel::setupForEngine(const EnginePlugin *engine, const GameMode &gameMode)
{
	setupDifficulty(engine);
	setupModifiers(engine);

	d->mapListBox->setVisible(engine->data()->hasMapList);
	d->mapListPanel->setupForEngine(engine);

	d->labelMaxClients->setVisible(engine->data()->allowsClientSlots);
	d->spinMaxClients->setVisible(engine->data()->allowsClientSlots);

	d->labelMaxPlayers->setVisible(engine->data()->allowsPlayerSlots);
	d->spinMaxPlayers->setVisible(engine->data()->allowsPlayerSlots);

	setupLimitWidgets(engine, gameMode);
	d->engine = engine;
}

void GameRulesPanel::setupForRemoteGame()
{
	QWidget *disableControls[] =
	{
		d->spinMaxClients, d->spinMaxPlayers, NULL
	};
	for (int i = 0; disableControls[i]; ++i)
		disableControls[i]->setDisabled(true);
}

void GameRulesPanel::setupDifficulty(const EnginePlugin *engine)
{
	QVariant oldLevel = d->cboDifficulty->itemData(d->cboDifficulty->currentIndex());
	if (oldLevel.isValid())
	{
		d->memorizedDifficulty = oldLevel;
	}
	d->cboDifficulty->clear();

	QList<GameCVar> levels = engine->data()->difficulty->get(QVariant());
	d->labelDifficulty->setVisible(!levels.isEmpty());
	d->cboDifficulty->setVisible(!levels.isEmpty());
	foreach (const GameCVar &level, levels)
	{
		d->cboDifficulty->addItem(level.name(), level.value());
		if (level.value() == d->memorizedDifficulty)
		{
			d->cboDifficulty->setCurrentIndex(d->cboDifficulty->count() - 1);
		}
	}
}

void GameRulesPanel::setupModifiers(const EnginePlugin *engine)
{
	d->cboModifier->clear();
	d->gameModifiers.clear();

	QList<GameCVar> modifiers = engine->gameModifiers();

	if (!modifiers.isEmpty())
	{
		d->cboModifier->show();
		d->labelModifiers->show();

		d->cboModifier->addItem(tr("< NONE >"));

		foreach (const GameCVar &cvar, modifiers)
		{
			d->cboModifier->addItem(cvar.name());
			d->gameModifiers << cvar;
		}
	}
	else
	{
		d->cboModifier->hide();
		d->labelModifiers->hide();
	}
}

void GameRulesPanel::removeLimitWidgets()
{
	foreach (PrivData<GameRulesPanel>::GameLimitWidget *widget, d->limitWidgets)
	{
		delete widget->label;
		delete widget->spinBox;
		delete widget;
	}

	d->limitWidgets.clear();
}

void GameRulesPanel::setupLimitWidgets(const EnginePlugin *engine, const GameMode &gameMode)
{
	memorizeLimits();
	removeLimitWidgets();
	QList<GameCVar> limits = engine->limits(gameMode);

	foreach (const GameCVar &limit, limits)
	{
		QLabel* label = new QLabel(this);
		label->setText(limit.name());
		QSpinBox* spinBox = new QSpinBox(this);
		spinBox->setMaximum(999999);
		spinBox->setValue(limit.value().toInt());

		d->limitsLayout->addRow(label, spinBox);

		PrivData<GameRulesPanel>::GameLimitWidget* glw = new PrivData<GameRulesPanel>::GameLimitWidget();
		glw->label = label;
		glw->spinBox = spinBox;
		glw->limit = limit;
		d->limitWidgets << glw;
	}

	loadMemorizedLimits(engine);
}
