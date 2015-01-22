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
#include "zandronumdmflags.h"
#include "zandronumgameinfo.h"

FlagsPageValueController::FlagsPageValueController(FlagsPage* flagsPage)
{
	this->flagsPage = flagsPage;

	FlagsPage* f = flagsPage;

	// Compat flags
	{
		QMap<unsigned, QCheckBox*>& m = compatflagsCheckboxes;

		// Compatibility
		// - Oldschool
		m.insert(ZandronumDmflags::COMPATF_MAGICSILENCE,
			f->cbCompatAllowSilentBFGTrick);
		m.insert(ZandronumDmflags::COMPATF_WALLRUN,
			f->cbCompatEnableWallRunning);
		m.insert(ZandronumDmflags::COMPATF_SILENTPICKUP,
			f->cbCompatDontLetOthersHearPickups);
		m.insert(ZandronumDmflags::COMPATF_INVISIBILITY,
			f->cbCompatMonstersSeeSemiInvisiblePlayers);
		// - General, left panel
		m.insert(ZandronumDmflags::COMPATF_DROPOFF,
			f->cbCompatNoMonstersDropoffMove);
		m.insert(ZandronumDmflags::COMPATF_BOOMSCROLL,
			f->cbCompatScrollingSectorsAreAdditive);
		m.insert(ZandronumDmflags::COMPATF_SILENT_INSTANT_FLOORS,
			f->cbCompatInstantlyMovingFloorsArentSilent);
		m.insert(ZandronumDmflags::COMPATF_SECTORSOUNDS,
			f->cbCompatSectorSoundsUseOriginalMethod);

		// - General, right panel
		m.insert(ZandronumDmflags::COMPATF_SHORTTEX,
			f->cbCompatFindShortestTexturesLikeDoom);
		m.insert(ZandronumDmflags::COMPATF_STAIRINDEX,
			f->cbCompatUseBuggierStairBuilding);
		m.insert(ZandronumDmflags::COMPATF_LIMITPAIN,
			f->cbCompatLimitPainElementals);
		m.insert(ZandronumDmflags::COMPATF_NO_PASSMOBJ,
			f->cbCompatActorsAreInfinitelyTall);
		m.insert(ZandronumDmflags::COMPATF_NOTOSSDROPS,
			f->cbCompatSpawnItemDropsOnTheFloor);
		m.insert(ZandronumDmflags::COMPATF_USEBLOCKING,
			f->cbCompatAllSpecialLinesCanDropUseLines);
		m.insert(ZandronumDmflags::COMPATF_NODOORLIGHT,
			f->cbCompatDisableBoomDoorLightEffect);
		m.insert(ZandronumDmflags::COMPATF_RAVENSCROLL,
			f->cbCompatRavensScrollersUseOriginalSpeed);
		m.insert(ZandronumDmflags::COMPATF_SOUNDTARGET,
			f->cbCompatUseSectorBasedSoundTargetCode);
		m.insert(ZandronumDmflags::COMPATF_DEHHEALTH,
			f->cbCompatLimitDehMaxHealthToHealthBonus);
		m.insert(ZandronumDmflags::COMPATF_TRACE,
			f->cbCompatTraceIgnoreLinesWithoutSameSectorOnBothSides);
		m.insert(ZandronumDmflags::COMPATF_CROSSDROPOFF,
			f->cbCompatMonstersCantBePushedOffCliffs);
		m.insert(ZandronumDmflags::COMPATF_MISSILECLIP,
			f->cbCompatUseOriginalMissileClippingHeight);
	}

	// Compat flags 2
	{
		QMap<unsigned, QCheckBox*>& m = zandronumCompatflagsCheckboxes;

		// Players
		m.insert(ZandronumDmflags::ZACOMPATF_DISABLETAUNTS,
			f->cbTaunt);
		m.insert(ZandronumDmflags::ZACOMPATF_NO_CROSSHAIR,
			f->cbUseCrosshairs);
		// - Disallow
		m.insert(ZandronumDmflags::ZACOMPATF_NO_LAND,
			f->cbUseLandConsoleCommand);
		// Compatibility
		m.insert(ZandronumDmflags::ZACOMPATF_PLASMA_BUMP_BUG,
			f->cbCompatPlasmaBumpBug);
		m.insert(ZandronumDmflags::ZACOMPATF_OLD_WEAPON_SWITCH,
			f->cbCompatOriginalWeaponSwitch);
		m.insert(ZandronumDmflags::ZACOMPATF_LIMITED_AIRMOVEMENT,
			f->cbCompatLimitedMovementInTheAir);
		m.insert(ZandronumDmflags::ZACOMPATF_INSTANTRESPAWN,
			f->cbCompatAllowInstantRespawn);
		m.insert(ZandronumDmflags::ZACOMPATF_OLDINTERMISSION,
			f->cbCompatUseOldIntermissionScreensMusic);
		m.insert(ZandronumDmflags::ZACOMPATF_DISABLESTEALTHMONSTERS,
			f->cbCompatDisableStealthMonsters);
		m.insert(ZandronumDmflags::ZACOMPATF_OLDRADIUSDMG,
			f->cbCompatOldDamageRadiusInfiniteHeight);
		m.insert(ZandronumDmflags::ZACOMPATF_CLIENTS_SEND_FULL_BUTTON_INFO,
			f->cbCompatClientsSendFullButtonInfo);
		m.insert(ZandronumDmflags::ZACOMPATF_OLD_RANDOM_GENERATOR,
			f->cbCompatOldRandomNumberGenerator);
		m.insert(ZandronumDmflags::ZACOMPATF_NETSCRIPTS_ARE_CLIENTSIDE,
			f->cbCompatNETScriptsAreClientside);
		m.insert(ZandronumDmflags::ZACOMPATF_NOGRAVITY_SPHERES,
			f->cbCompatAddNOGRAVITYFlagToSpheres);
		m.insert(ZandronumDmflags::ZACOMPATF_DONT_STOP_PLAYER_SCRIPTS_ON_DISCONNECT,
			f->cbDontStopPlayerScriptsOnDisconnect);
		m.insert(ZandronumDmflags::ZACOMPATF_OLD_EXPLOSION_THRUST,
			f->cbOldZDoomHorizontalThrust);
		m.insert(ZandronumDmflags::ZACOMPATF_OLD_BRIDGE_DROPS,
			f->cbOldZDoomBridgeDrops);
		m.insert(ZandronumDmflags::ZACOMPATF_OLD_ZDOOM_ZMOVEMENT,
			f->cbOldZDoomJumpPhysics);
		m.insert(ZandronumDmflags::ZACOMPATF_FULL_WEAPON_LOWER,
			f->cbCompatFullWeaponLower);
		m.insert(ZandronumDmflags::ZACOMPATF_ORIGINALSOUNDCURVE,
			f->cbCompatOriginalSoundCurve);
	}

	// DMFlags
	{
		QMap<unsigned, QCheckBox*>& m = dmflagsCheckboxes;

		// General
		m.insert(ZandronumDmflags::DF_NO_MONSTERS,
			f->cbNoMonsters);
		m.insert(ZandronumDmflags::DF_ITEMS_RESPAWN,
			f->cbItemsRespawn);
		m.insert(ZandronumDmflags::DF_RESPAWN_SUPER,
			f->cbMegaPowerupsRespawn);

		// Players
		m.insert(ZandronumDmflags::DF_NO_FOV,
			f->cbUseFOV);
		m.insert(ZandronumDmflags::DF_NO_FREELOOK,
			f->cbUseFreelook);
		m.insert(ZandronumDmflags::DF_INFINITE_AMMO,
			f->cbInfiniteAmmo);

		// Cooperative
		// - Environment
		m.insert(ZandronumDmflags::DF_NO_COOP_WEAPON_SPAWN,
			f->cbDontSpawnDeathmatchWeapons);
		// - Monsters
		m.insert(ZandronumDmflags::DF_FAST_MONSTERS,
			f->cbMonstersAreFast);
		m.insert(ZandronumDmflags::DF_MONSTERS_RESPAWN,
			f->cbMonstersRespawn);
		// - Player death
		m.insert(ZandronumDmflags::DF_COOP_LOSE_INVENTORY,
			f->cbLoseAllInventory);
		m.insert(ZandronumDmflags::DF_COOP_LOSE_ARMOR,
			f->cbLoseArmor);
		m.insert(ZandronumDmflags::DF_COOP_LOSE_KEYS,
			f->cbLoseKeys);
		m.insert(ZandronumDmflags::DF_COOP_LOSE_POWERUPS,
			f->cbLosePowerups);
		m.insert(ZandronumDmflags::DF_COOP_LOSE_WEAPONS,
			f->cbLoseWeapons);
		m.insert(ZandronumDmflags::DF_COOP_LOSE_AMMO,
			f->cbLoseAllAmmo);
		m.insert(ZandronumDmflags::DF_COOP_HALVE_AMMO,
			f->cbLoseHalfAmmo);

		// Deathmatch
		// - When players die
		m.insert(ZandronumDmflags::DF_FORCE_RESPAWN,
			f->cbRespawnAutomatically);
		m.insert(ZandronumDmflags::DF_SPAWN_FARTHEST,
			f->cbRespawnFarthestAwayFromOthers);
		// - Weapons&ammo
		m.insert(ZandronumDmflags::DF_WEAPONS_STAY,
			f->cbWeaponsStayAfterPickup);
		// - Don't spawn
		m.insert(ZandronumDmflags::DF_NO_HEALTH,
			f->cbDontSpawnHealth);
		m.insert(ZandronumDmflags::DF_NO_ARMOR,
			f->cbDontSpawnArmor);
	}

	// DMFlags 2
	{
		QMap<unsigned, QCheckBox*>& m = dmflags2Checkboxes;

		// General
		m.insert(ZandronumDmflags::DF2_BARRELS_RESPAWN,
			f->cbBarrelsRespawn);
		m.insert(ZandronumDmflags::DF2_NO_TEAM_SELECT,
			f->cbServerPicksTeams);
		m.insert(ZandronumDmflags::DF2_NO_TEAM_SWITCH,
			f->cbPlayersCantSwitchTeams);
		m.insert(ZandronumDmflags::ZADF_YES_KEEP_TEAMS,
			f->cbKeepTeamsAfterAMapChange);
		m.insert(ZandronumDmflags::DF2_NO_AUTOMAP_ALLIES,
			f->cbHideAlliesOnTheAutomap);
		m.insert(ZandronumDmflags::DF2_DISALLOW_SPYING,
			f->cbDontLetPlayersSpyOnAllies);
		m.insert(ZandronumDmflags::DF2_INSTANT_RETURN,
			f->cbInstantFlagSkullReturn);

		// Players
		// - Disallow
		m.insert(ZandronumDmflags::DF2_NOSUICIDE,
			f->cbSuicide);
		m.insert(ZandronumDmflags::DF2_NO_RESPAWN,
			f->cbRespawn);
		m.insert(ZandronumDmflags::DF2_NO_AUTOMAP,
			f->cbUseAutomap);
		m.insert(ZandronumDmflags::DF2_NOAUTOAIM,
			f->cbUseAutoaim);
		// - Abilities
		m.insert(ZandronumDmflags::DF2_INFINITE_INVENTORY,
			f->cbInfiniteInventory);
		m.insert(ZandronumDmflags::DF2_YES_DEGENERATION,
			f->cbSlowlyLoseHealthWhenOver100);
		m.insert(ZandronumDmflags::DF2_CHASECAM,
			f->cbCanUseChasecam);
		m.insert(ZandronumDmflags::DF2_YES_FREEAIMBFG,
			f->cbAllowBFGFreeaiming);

		// Cooperative
		// - Monsters
		m.insert(ZandronumDmflags::DF2_KILL_MONSTERS,
			f->cbMonstersMustBeKilledToExit);
		// - Player death
		m.insert(ZandronumDmflags::DF2_SAME_SPAWN_SPOT,
			f->cbRespawnWhereDied);

		// Deathmatch
		// - When players die
		m.insert(ZandronumDmflags::DF2_COOP_SHOTGUNSTART,
			f->cbRespawnWithAShotgun);
		m.insert(ZandronumDmflags::DF2_YES_WEAPONDROP,
			f->cbDropWeaponOnDeath);
		m.insert(ZandronumDmflags::DF2_YES_LOSEFRAG,
			f->cbLoseAFragOnDeath);
		m.insert(ZandronumDmflags::DF2_NO_RESPAWN_INVUL,
			f->cbNoRespawnProtection);
		// - When someone exits
		m.insert(ZandronumDmflags::DF2_YES_KEEPFRAGS,
			f->cbKeepFragsAfterMapChange);
		// - Weapons&ammo
		m.insert(ZandronumDmflags::DF2_YES_DOUBLEAMMO,
			f->cbDoubleAmmo);
		// - Don't spawn
		m.insert(ZandronumDmflags::DF2_NO_RUNES,
			f->cbDontSpawnRunes);
	}

	// DMFlags 3
	{
		QMap<unsigned, QCheckBox*>& m = zandronumDmflagsCheckboxes;

		// General
		m.insert(ZandronumDmflags::ZADF_NOUNLAGGED,
			f->cbNoUnlagged);
		m.insert(ZandronumDmflags::ZADF_ALWAYS_APPLY_LMS_SPECTATORSETTINGS,
			f->cbAlwaysApplyLMSSpectatorSettings);
		m.insert(ZandronumDmflags::ZADF_NO_MEDALS,
			f->cbNoMedals);
		m.insert(ZandronumDmflags::ZADF_AWARD_DAMAGE_INSTEAD_KILLS,
			f->cbScoreDamageNotKills);
		// - Environment
		m.insert(ZandronumDmflags::ZADF_COOP_SP_ACTOR_SPAWN,
			f->cbDontSpawnAnyMultiplayerItem);

		// Players
		// - Disallow 2
		m.insert(ZandronumDmflags::ZADF_NO_IDENTIFY_TARGET,
			f->cbTargetIdentify);
		m.insert(ZandronumDmflags::ZADF_NO_COOP_INFO,
			f->cbDisplayCoopInfo);
		// - Behavior
		m.insert(ZandronumDmflags::ZADF_UNBLOCK_PLAYERS,
			f->cbPlayersCanWalkThroughEachOther);

		m.insert(ZandronumDmflags::ZADF_NO_ROCKET_JUMPING,
			f->cbRocketJump);
		m.insert(ZandronumDmflags::ZADF_FORCE_GL_DEFAULTS,
			f->cbUseCustomGLLightingSettings);
		m.insert(ZandronumDmflags::ZADF_FORCE_ALPHA,
			f->cbTurnOffTranslucency);
	}

	// LMSAllowedWeapons
	{
		QMap<unsigned, QCheckBox*>& m = lmsAllowedWeaponsCheckboxes;

		m.insert(ZandronumGameInfo::LMSAW_Chainsaw,
			f->cbLMSChainsaw);
		m.insert(ZandronumGameInfo::LMSAW_Pistol,
			f->cbLMSPistol);
		m.insert(ZandronumGameInfo::LMSAW_Shotgun,
			f->cbLMSShotgun);
		m.insert(ZandronumGameInfo::LMSAW_SuperShotgun,
			f->cbLMSSuperShotgun);
		m.insert(ZandronumGameInfo::LMSAW_Chaingun,
			f->cbLMSChaingun);
		m.insert(ZandronumGameInfo::LMSAW_Minigun,
			f->cbLMSMinigun);
		m.insert(ZandronumGameInfo::LMSAW_RocketLauncher,
			f->cbLMSRocketLauncher);
		m.insert(ZandronumGameInfo::LMSAW_GrenadeLauncher,
			f->cbLMSGrenadeLauncher);
		m.insert(ZandronumGameInfo::LMSAW_PlasmaRifle,
			f->cbLMSPlasmaRifle);
		m.insert(ZandronumGameInfo::LMSAW_Railgun,
			f->cbLMSRailgun);
	}

	// LMSSpectatorSettings
	{
		QMap<unsigned, QCheckBox*>& m = lmsSpectatorSettingsCheckboxes;

		m.insert(ZandronumGameInfo::LMSSS_TalkToActivePlayers,
			f->cbLMSSpectatorsCanTalkToActivePlayers);
		m.insert(ZandronumGameInfo::LMSSS_ViewTheGame,
			f->cbLMSSpectatorsCanViewTheGame);
	}
}

void FlagsPageValueController::convertNumericalToWidgets()
{
	// Get values from widgets.
	readFlagsFromTextInputs();

	convertNumericalToCheckboxes(compatflagsCheckboxes, compatflags);
	convertNumericalToCheckboxes(zandronumCompatflagsCheckboxes, zandronumCompatflags);
	convertNumericalToCheckboxes(dmflagsCheckboxes, dmflags);
	convertNumericalToCheckboxes(dmflags2Checkboxes, dmflags2);
	convertNumericalToCheckboxes(zandronumDmflagsCheckboxes, zandronumDmflags);
	convertNumericalToCheckboxes(lmsAllowedWeaponsCheckboxes, lmsAllowedWeapons);
	convertNumericalToCheckboxes(lmsSpectatorSettingsCheckboxes, lmsSpectatorSettings);

	convertToWidgetGeneral();
	convertToWidgetPlayers();
	convertToWidgetCooperative();
	convertToWidgetDeathmatch();
}

void FlagsPageValueController::convertWidgetsToNumerical()
{
	// Reset internal values.
	readFlagsFromTextInputs();

	convertCheckboxesToNumerical(compatflagsCheckboxes, compatflags);
	convertCheckboxesToNumerical(zandronumCompatflagsCheckboxes, zandronumCompatflags);
	convertCheckboxesToNumerical(dmflagsCheckboxes, dmflags);
	convertCheckboxesToNumerical(dmflags2Checkboxes, dmflags2);
	convertCheckboxesToNumerical(zandronumDmflagsCheckboxes, zandronumDmflags);
	convertCheckboxesToNumerical(lmsAllowedWeaponsCheckboxes, lmsAllowedWeapons);
	convertCheckboxesToNumerical(lmsSpectatorSettingsCheckboxes, lmsSpectatorSettings);

	convertToNumericalGeneral();
	convertToNumericalPlayers();
	convertToNumericalCooperative();
	convertToNumericalDeathmatch();

	// Apply values to widgets.
	flagsPage->leDmflags->setText(QString::number(dmflags));
	flagsPage->leDmflags2->setText(QString::number(dmflags2));
	flagsPage->leZandronumDmflags->setText(QString::number(zandronumDmflags));
	flagsPage->leCompatflags->setText(QString::number(compatflags));
	flagsPage->leZandronumCompatflags->setText(QString::number(zandronumCompatflags));
	flagsPage->leLMSAllowedWeapons->setText(QString::number(lmsAllowedWeapons));
	flagsPage->leLMSSpectatorSettings->setText(QString::number(lmsSpectatorSettings));
}

void FlagsPageValueController::convertCheckboxesToNumerical(
	const QMap<unsigned, QCheckBox*>& checkboxMap,
	unsigned& flagsValue)
{
	foreach (unsigned flag, checkboxMap.keys())
	{

		if (checkboxMap[flag]->isChecked())
		{
			flagsValue |= flag;
		}
		else
		{
			// We are subtracting the disabled flags from the current value,
			// because the flagsValue variable preserves user input flags which
			// cannot are not supported by the GUI.
			flagsValue &= ~flag;
		}

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

	// Reset the value.
	dmflags &= ~(ZandronumDmflags::DF_FORCE_FALLINGZD
				| ZandronumDmflags::DF_FORCE_FALLINGHX
				| ZandronumDmflags::DF_FORCE_FALLINGST);
	switch (f->cboFallingDamage->currentIndex())
	{
		case FlagsPage::FDT_None:
			// pass
			break;

		case FlagsPage::FDT_Old:
			dmflags |= ZandronumDmflags::DF_FORCE_FALLINGZD;
			break;

		case FlagsPage::FDT_Hexen:
			dmflags |= ZandronumDmflags::DF_FORCE_FALLINGHX;
			break;

		case FlagsPage::FDT_Strife:
			dmflags |= ZandronumDmflags::DF_FORCE_FALLINGST;
			break;
	}
}
////////////////////////////////////////////////////////////////////////////////
void FlagsPageValueController::convertToNumericalPlayers()
{
	FlagsPage* f = flagsPage;

	// Abilities

	// Reset the value.
	dmflags &= ~(ZandronumDmflags::DF_YES_JUMP
				| ZandronumDmflags::DF_NO_JUMP);
	switch (f->cboJumping->currentIndex())
	{
		case FlagsPage::JCA_Default:
			// pass
			break;
		case FlagsPage::JCA_Yes:
			dmflags |= ZandronumDmflags::DF_YES_JUMP;
			break;
		case FlagsPage::JCA_No:
			dmflags |= ZandronumDmflags::DF_NO_JUMP;
			break;
	}

	// Reset the value.
	dmflags &= ~(ZandronumDmflags::DF_YES_CROUCH
				| ZandronumDmflags::DF_NO_CROUCH);
	switch (f->cboCrouching->currentIndex())
	{
		case FlagsPage::JCA_Default:
			// pass
			break;
		case FlagsPage::JCA_Yes:
			dmflags |= ZandronumDmflags::DF_YES_CROUCH;
			break;
		case FlagsPage::JCA_No:
			dmflags |= ZandronumDmflags::DF_NO_CROUCH;
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
void FlagsPageValueController::convertToNumericalDeathmatch()
{
	FlagsPage* f = flagsPage;

	// Reset the value.
	dmflags &= ~(ZandronumDmflags::DF_NO_EXIT
			| ZandronumDmflags::DF_SAME_LEVEL);
	if (f->rbKillThePlayer->isChecked())
	{
		dmflags |= ZandronumDmflags::DF_NO_EXIT;
	}
	else if (f->rbRestartTheCurrentLevel->isChecked())
	{
		dmflags |= ZandronumDmflags::DF_SAME_LEVEL;
	}
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
	if ((dmflags & ZandronumDmflags::DF_FORCE_FALLINGST)
		== ZandronumDmflags::DF_FORCE_FALLINGST)
	{
		f->cboFallingDamage->setCurrentIndex(FlagsPage::FDT_Strife);
	}
	else if(dmflags & ZandronumDmflags::DF_FORCE_FALLINGZD)
	{
		f->cboFallingDamage->setCurrentIndex(FlagsPage::FDT_Old);
	}
	else if (dmflags & ZandronumDmflags::DF_FORCE_FALLINGHX)
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
	if (dmflags & ZandronumDmflags::DF_YES_JUMP)
	{
		f->cboJumping->setCurrentIndex(FlagsPage::JCA_Yes);
	}
	else if (dmflags & ZandronumDmflags::DF_NO_JUMP)
	{
		f->cboJumping->setCurrentIndex(FlagsPage::JCA_No);
	}
	else
	{
		f->cboJumping->setCurrentIndex(FlagsPage::JCA_Default);
	}

	if (dmflags & ZandronumDmflags::DF_YES_CROUCH)
	{
		f->cboCrouching->setCurrentIndex(FlagsPage::JCA_Yes);
	}
	else if (dmflags & ZandronumDmflags::DF_NO_CROUCH)
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
////////////////////////////////////////////////////////////////////////////////
void FlagsPageValueController::convertToWidgetDeathmatch()
{
	FlagsPage* f = flagsPage;

	if (dmflags & ZandronumDmflags::DF_NO_EXIT)
	{
		f->rbKillThePlayer->setChecked(true);
	}
	else if (dmflags & ZandronumDmflags::DF_SAME_LEVEL)
	{
		f->rbRestartTheCurrentLevel->setChecked(true);
	}
	else
	{
		f->rbContinueToTheNextMap->setChecked(true);
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void FlagsPageValueController::readFlagsFromTextInputs()
{
	dmflags = flagsPage->leDmflags->text().toUInt();
	dmflags2 = flagsPage->leDmflags2->text().toUInt();
	zandronumDmflags = flagsPage->leZandronumDmflags->text().toUInt();
	compatflags = flagsPage->leCompatflags->text().toUInt();
	zandronumCompatflags = flagsPage->leZandronumCompatflags->text().toUInt();
	lmsAllowedWeapons = flagsPage->leLMSAllowedWeapons->text().toUInt();
	lmsSpectatorSettings = flagsPage->leLMSSpectatorSettings->text().toUInt();
}
