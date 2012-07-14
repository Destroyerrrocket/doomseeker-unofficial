//------------------------------------------------------------------------------
// flagspagevaluecontroller.cpp
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "flagspagevaluecontroller.h"

#include "createserverdialogpages/flagspage.h"
#include "zandronumgameinfo.h"

FlagsPageValueController::FlagsPageValueController(FlagsPage* flagsPage)
{
	this->flagsPage = flagsPage;

	FlagsPage* f = flagsPage;

	// Compat flags
	{
		QMap<unsigned, QCheckBox*>& m = compatflagsCheckboxes;

		// Players.
		m.insert(ZandronumGameInfo::COMPATF_DISABLETAUNTS,
			f->cbTaunt);
		m.insert(ZandronumGameInfo::COMPATF_NO_CROSSHAIR,
			f->cbUseCrosshairs);
	}

	// Compat flags 2
	{
		QMap<unsigned, QCheckBox*>& m = compatflags2Checkboxes;


	}

	// DMFlags
	{
		QMap<unsigned, QCheckBox*>& m = dmflagsCheckboxes;

		// General
		m.insert(ZandronumGameInfo::DF_NO_MONSTERS,
			f->cbNoMonsters);
		m.insert(ZandronumGameInfo::DF_ITEMS_RESPAWN,
			f->cbItemsRespawn);
		m.insert(ZandronumGameInfo::DF_RESPAWN_SUPER,
			f->cbMegaPowerupsRespawn);

		// Players
		m.insert(ZandronumGameInfo::DF_NO_FOV,
			f->cbUseFOV);
		m.insert(ZandronumGameInfo::DF_NO_FREELOOK,
			f->cbUseFreelook);
		m.insert(ZandronumGameInfo::DF_INFINITE_AMMO,
			f->cbInfiniteAmmo);

		// Cooperative
		// - Environment
		m.insert(ZandronumGameInfo::DF_NO_COOP_WEAPON_SPAWN,
			f->cbDontSpawnDeathmatchWeapons);
		// - Monsters
		m.insert(ZandronumGameInfo::DF_FAST_MONSTERS,
			f->cbMonstersAreFast);
		m.insert(ZandronumGameInfo::DF_MONSTERS_RESPAWN,
			f->cbMonstersRespawn);
		// - Player death
		m.insert(ZandronumGameInfo::DF_COOP_LOSE_INVENTORY,
			f->cbLoseAllInventory);
		m.insert(ZandronumGameInfo::DF_COOP_LOSE_ARMOR,
			f->cbLoseArmor);
		m.insert(ZandronumGameInfo::DF_COOP_LOSE_KEYS,
			f->cbLoseKeys);
		m.insert(ZandronumGameInfo::DF_COOP_LOSE_POWERUPS,
			f->cbLosePowerups);
		m.insert(ZandronumGameInfo::DF_COOP_LOSE_WEAPONS,
			f->cbLoseWeapons);
		m.insert(ZandronumGameInfo::DF_COOP_LOSE_AMMO,
			f->cbLoseAllAmmo);
		m.insert(ZandronumGameInfo::DF_COOP_HALVE_AMMO,
			f->cbLoseHalfAmmo);
	}

	// DMFlags 2
	{
		QMap<unsigned, QCheckBox*>& m = dmflags2Checkboxes;

		// General
		m.insert(ZandronumGameInfo::DF2_BARRELS_RESPAWN,
			f->cbBarrelsRespawn);
		m.insert(ZandronumGameInfo::DF2_NO_TEAM_SELECT,
			f->cbServerPicksTeams);
		m.insert(ZandronumGameInfo::DF2_NO_TEAM_SWITCH,
			f->cbPlayersCantSwitchTeams);
		m.insert(ZandronumGameInfo::DF2_YES_KEEP_TEAMS,
			f->cbKeepTeamsAfterAMapChange);
		m.insert(ZandronumGameInfo::DF2_NO_AUTOMAP_ALLIES,
			f->cbHideAlliesOnTheAutomap);
		m.insert(ZandronumGameInfo::DF2_DISALLOW_SPYING,
			f->cbDontLetPlayersSpyOnAllies);
		m.insert(ZandronumGameInfo::DF2_INSTANT_RETURN,
			f->cbInstantFlagSkullReturn);

		// Players
		// - Disallow
		m.insert(ZandronumGameInfo::DF2_NOSUICIDE,
			f->cbSuicide);
		m.insert(ZandronumGameInfo::DF2_NO_RESPAWN,
			f->cbRespawn);
		m.insert(ZandronumGameInfo::DF2_NO_ROCKET_JUMPING,
			f->cbRocketJump);
		m.insert(ZandronumGameInfo::DF2_NO_AUTOMAP,
			f->cbUseAutomap);
		m.insert(ZandronumGameInfo::DF2_FORCE_GL_DEFAULTS,
			f->cbUseCustomGLLightingSettings);
		m.insert(ZandronumGameInfo::DF2_FORCE_ALPHA,
			f->cbTurnOffTranslucency);
		m.insert(ZandronumGameInfo::DF2_NOAUTOAIM,
			f->cbUseAutoaim);
		// - Abilities
		m.insert(ZandronumGameInfo::DF2_INFINITE_INVENTORY,
			f->cbInfiniteInventory);
		m.insert(ZandronumGameInfo::DF2_YES_DEGENERATION,
			f->cbSlowlyLoseHealthWhenOver100);
		m.insert(ZandronumGameInfo::DF2_CHASECAM,
			f->cbCanUseChasecam);
		m.insert(ZandronumGameInfo::DF2_YES_FREEAIMBFG,
			f->cbAllowBFGFreeaiming);

		// Cooperative
		// - General
		m.insert(ZandronumGameInfo::DF2_AWARD_DAMAGE_INSTEAD_KILLS,
			f->cbScoreDamageNotKills);
		// - Monsters
		m.insert(ZandronumGameInfo::DF2_KILL_MONSTERS,
			f->cbMonstersMustBeKilledToExit);
		// - Player death
		m.insert(ZandronumGameInfo::DF2_SAME_SPAWN_SPOT,
			f->cbRespawnWhereDied);
	}

	// DMFlags 3
	{
		QMap<unsigned, QCheckBox*>& m = dmflags3Checkboxes;

		// General
		m.insert(ZandronumGameInfo::DF3_NOUNLAGGED,
			f->cbNoUnlagged);
		m.insert(ZandronumGameInfo::DF3_ALWAYS_APPLY_LMS_SPECTATORSETTINGS,
			f->cbAlwaysApplyLMSSpectatorSettings);

		// Players
		// - Disallow 2
		m.insert(ZandronumGameInfo::DF3_NO_IDENTIFY_TARGET,
			f->cbTargetIdentify);
		m.insert(ZandronumGameInfo::DF3_NO_COOP_INFO,
			f->cbDisplayCoopInfo);
		// - Behavior
		m.insert(ZandronumGameInfo::DF3_UNBLOCK_PLAYERS,
			f->cbPlayersCanWalkThroughEachOther);
	}
}

void FlagsPageValueController::convertNumericalToWidgets()
{
	// Get values from widgets.
	dmflags = flagsPage->leDmflags->text().toUInt();
	dmflags2 = flagsPage->leDmflags2->text().toUInt();
	dmflags3 = flagsPage->leDmflags3->text().toUInt();
	compatflags = flagsPage->leCompatflags->text().toUInt();
	compatflags2 = flagsPage->leCompatflags2->text().toUInt();

	convertNumericalToCheckboxes(compatflagsCheckboxes, compatflags);
	convertNumericalToCheckboxes(compatflags2Checkboxes, compatflags2);
	convertNumericalToCheckboxes(dmflagsCheckboxes, dmflags);
	convertNumericalToCheckboxes(dmflags2Checkboxes, dmflags2);
	convertNumericalToCheckboxes(dmflags3Checkboxes, dmflags3);

	convertToWidgetGeneral();
	convertToWidgetPlayers();
	convertToWidgetCooperative();
}

void FlagsPageValueController::convertWidgetsToNumerical()
{
	// Reset internal values.
	dmflags = 0;
	dmflags2 = 0;
	dmflags3 = 0;
	compatflags = 0;
	compatflags2 = 0;

	convertCheckboxesToNumerical(compatflagsCheckboxes, compatflags);
	convertCheckboxesToNumerical(compatflags2Checkboxes, compatflags2);
	convertCheckboxesToNumerical(dmflagsCheckboxes, dmflags);
	convertCheckboxesToNumerical(dmflags2Checkboxes, dmflags2);
	convertCheckboxesToNumerical(dmflags3Checkboxes, dmflags3);

	convertToNumericalGeneral();
	convertToNumericalPlayers();
	convertToNumericalCooperative();

	// Apply values to widgets.
	flagsPage->leDmflags->setText(QString::number(dmflags));
	flagsPage->leDmflags2->setText(QString::number(dmflags2));
	flagsPage->leDmflags3->setText(QString::number(dmflags3));
	flagsPage->leCompatflags->setText(QString::number(compatflags));
	flagsPage->leCompatflags2->setText(QString::number(compatflags2));
}

void FlagsPageValueController::convertCheckboxesToNumerical(
	const QMap<unsigned, QCheckBox*>& checkboxMap,
	unsigned& flagsValue)
{
	foreach (unsigned flag, checkboxMap.keys())
	{
		flagsValue |= checkboxMap[flag]->isChecked() ? flag : 0;
	}
}

void FlagsPageValueController::convertNumericalToCheckboxes(
	QMap<unsigned, QCheckBox*>& checkboxMap,
	unsigned flagsValue)
{
	foreach (unsigned flag, checkboxMap.keys())
	{
		checkboxMap[flag]->setChecked(flagsValue & flag);
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void FlagsPageValueController::convertToNumericalGeneral()
{
	FlagsPage* f = flagsPage;

	// Environment
	switch (f->cboFallingDamage->currentIndex())
	{
		case FlagsPage::FDT_None:
			// pass
			break;

		case FlagsPage::FDT_Old:
			dmflags |= ZandronumGameInfo::DF_FORCE_FALLINGZD;
			break;

		case FlagsPage::FDT_Hexen:
			dmflags |= ZandronumGameInfo::DF_FORCE_FALLINGHX;
			break;

		case FlagsPage::FDT_Strife:
			dmflags |= ZandronumGameInfo::DF_FORCE_FALLINGST;
			break;
	}
}
////////////////////////////////////////////////////////////////////////////////
void FlagsPageValueController::convertToNumericalPlayers()
{
	FlagsPage* f = flagsPage;

	// Abilities
	switch (f->cboJumping->currentIndex())
	{
		case FlagsPage::JCA_Default:
			// Do nothing
			break;
		case FlagsPage::JCA_Yes:
			dmflags |= ZandronumGameInfo::DF_YES_JUMP;
			break;
		case FlagsPage::JCA_No:
			dmflags |= ZandronumGameInfo::DF_NO_JUMP;
			break;
	}

	switch (f->cboCrouching->currentIndex())
	{
		case FlagsPage::JCA_Default:
			// Do nothing
			break;
		case FlagsPage::JCA_Yes:
			dmflags |= ZandronumGameInfo::DF_YES_CROUCH;
			break;
		case FlagsPage::JCA_No:
			dmflags |= ZandronumGameInfo::DF_NO_CROUCH;
			break;
	}
}
////////////////////////////////////////////////////////////////////////////////
void FlagsPageValueController::convertToNumericalCooperative()
{
	FlagsPage* f = flagsPage;

	// Placeholder method for future use.
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void FlagsPageValueController::convertToWidgetGeneral()
{
	FlagsPage* f = flagsPage;

	// Environment

	// This one is tricky because FALLINGST flag is actually defined
	// as a combination of two other flags. We need to check for it first and
	// we also need to check if the result of 'AND' operation is equal to the
	// value of the flag.
	if ((dmflags & ZandronumGameInfo::DF_FORCE_FALLINGST)
		== ZandronumGameInfo::DF_FORCE_FALLINGST)
	{
		f->cboFallingDamage->setCurrentIndex(FlagsPage::FDT_Strife);
	}
	else if(dmflags & ZandronumGameInfo::DF_FORCE_FALLINGZD)
	{
		f->cboFallingDamage->setCurrentIndex(FlagsPage::FDT_Old);
	}
	else if (dmflags & ZandronumGameInfo::DF_FORCE_FALLINGHX)
	{
		f->cboFallingDamage->setCurrentIndex(FlagsPage::FDT_Hexen);
	}
	else
	{
		f->cboFallingDamage->setCurrentIndex(FlagsPage::FDT_None);
	}
}
////////////////////////////////////////////////////////////////////////////////
void FlagsPageValueController::convertToWidgetPlayers()
{
	FlagsPage* f = flagsPage;

	// Abilities
	if (dmflags & ZandronumGameInfo::DF_YES_JUMP)
	{
		f->cboJumping->setCurrentIndex(FlagsPage::JCA_Yes);
	}
	else if (dmflags & ZandronumGameInfo::DF_NO_JUMP)
	{
		f->cboJumping->setCurrentIndex(FlagsPage::JCA_No);
	}
	else
	{
		f->cboJumping->setCurrentIndex(FlagsPage::JCA_Default);
	}

	if (dmflags & ZandronumGameInfo::DF_YES_CROUCH)
	{
		f->cboCrouching->setCurrentIndex(FlagsPage::JCA_Yes);
	}
	else if (dmflags & ZandronumGameInfo::DF_NO_CROUCH)
	{
		f->cboCrouching->setCurrentIndex(FlagsPage::JCA_No);
	}
	else
	{
		f->cboCrouching->setCurrentIndex(FlagsPage::JCA_Default);
	}
}
////////////////////////////////////////////////////////////////////////////////
void FlagsPageValueController::convertToWidgetCooperative()
{
	FlagsPage* f = flagsPage;

	// Placeholder method for future use.
}
