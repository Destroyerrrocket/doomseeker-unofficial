#include "flagspage.h"

#include <QDebug>
#include <QSharedPointer>
#include <QValidator>
#include <ini/ini.h>
#include <ini/inisection.h>
#include <log.h>

#include "createserverdialogpages/flagsid.h"
#include "createserverdialogpages/flagspagevaluecontroller2.h"
#include "createserverdialogpages/flagspagevaluecontroller3.h"
#include "createserverdialogpages/votingsetupwidget.h"
#include "zandronumgameinfo.h"

const unsigned DEFAULT_LMSALLOWEDWEAPONS = 1023;
const unsigned DEFAULT_LMSSPECTATORSETTINGS = 3;

class DmflagsValidator : public QValidator
{
	public:
		void fixup(QString& input) const
		{
			if (input.trimmed().isEmpty())
			{
				input = "0";
			}
		}

		State validate(QString& input, int& pos ) const
		{
			if (input.trimmed().isEmpty())
			{
				return QValidator::Intermediate;
			}

			bool bOk;
			input.toUInt(&bOk);
			return bOk ? QValidator::Acceptable : QValidator::Invalid;
		}
};
////////////////////////////////////////////////////////////////////////////////
class FlagsPage::PrivData
{
public:
	DmflagsValidator validator;
	QSharedPointer<FlagsPageValueController> flagsController;
};

////////////////////////////////////////////////////////////////////////////////
FlagsPage::FlagsPage(CreateServerDialog* pParentDialog)
: CreateServerDialogPage(pParentDialog, tr("Flags"))
{
	setupUi(this);

	d = new PrivData();
	FlagsId flagsId(this);
	flagsId.assign();

	// The validator makes sure that the line edit will accept only
	// 32-bit unsigned values.
	leDmflags->setValidator(&d->validator);
	leDmflags2->setValidator(&d->validator);
	leZandronumDmflags->setValidator(&d->validator);
	leCompatflags->setValidator(&d->validator);
	leCompatflags2->setValidator(&d->validator);
	leZandronumCompatflags->setValidator(&d->validator);
	leLMSAllowedWeapons->setValidator(&d->validator);
	leLMSSpectatorSettings->setValidator(&d->validator);

	// Init values for widgets.
	cboGameVersion->addItem(tr("Zandronum 2"), GV_Zandronum2);
	cboGameVersion->addItem(tr("Zandronum 3 (beta)"), GV_Zandronum3);

	cboFallingDamage->insertItem(FDT_None, tr("None"));
	cboFallingDamage->insertItem(FDT_Old, tr("Old (ZDoom)"));
	cboFallingDamage->insertItem(FDT_Hexen, tr("Hexen"));
	cboFallingDamage->insertItem(FDT_Strife, tr("Strife"));
	cboFallingDamage->setCurrentIndex(FDT_None);

	initJumpCrouchComboBoxes(cboJumping);
	initJumpCrouchComboBoxes(cboCrouching);

	setGameVersion(GV_Zandronum2);

	// Widget states
	spinMonsterKillPercentage->setEnabled(false);
}

FlagsPage::~FlagsPage()
{
	delete d;
}

void FlagsPage::applyWidgetsChange()
{
	if (d->flagsController != NULL)
		d->flagsController->convertWidgetsToNumerical();
}

QStringList FlagsPage::generateGameRunParameters()
{
	QStringList params;

	params << "+dmflags" << leDmflags->text();
	params << "+dmflags2" << leDmflags2->text();
	params << "+zadmflags" << leZandronumDmflags->text();
	params << "+compatflags" << leCompatflags->text();
	if (leCompatflags2->isEnabled() && leCompatflags2->isVisible())
		params << "+compatflags2" << leCompatflags2->text();
	params << "+zacompatflags" << leZandronumCompatflags->text();
	params << "+lmsallowedweapons" << leLMSAllowedWeapons->text();
	params << "+lmsspectatorsettings" << leLMSSpectatorSettings->text();

	if (cbMonstersMustBeKilledToExit->isChecked())
	{
		params << "+sv_killallmonsters_percentage"
			<< QString::number(spinMonsterKillPercentage->value());
	}
	params << "+sv_afk2spec" <<
		QString::number(spinForceInactivePlayersSpectatingMins->value());
	params << "+sv_coop_damagefactor" <<
		QString::number(spinMonstersDamageFactor->value());

	params << "+sv_defaultdmflags" << (cbDefaultDmflags->isChecked() ? "1" : "0");

	params << votingPage->generateGameRunParameters();

	return params;
}

void FlagsPage::initJumpCrouchComboBoxes(QComboBox* pComboBox)
{
	pComboBox->insertItem(JCA_Default, tr("Default"));
	pComboBox->insertItem(JCA_No, tr("No"));
	pComboBox->insertItem(JCA_Yes, tr("Yes"));
}

void FlagsPage::insertFlagsIfValid(QLineEdit* dst, QString flags, unsigned valIfInvalid)
{
	int dummy;
	if (d->validator.validate(flags, dummy) == QValidator::Acceptable)
	{
		dst->setText(flags);
	}
	else
	{
		dst->setText(QString::number(valIfInvalid));
	}
}

bool FlagsPage::loadConfig(Ini& ini)
{
	IniSection section = ini.section("dmflags");
	loadGameVersion(static_cast<GameVersion>((int) section["gameversion"]));

	// The below numerical flag inserts are here to support old configs.
	insertFlagsIfValid(leDmflags, section["dmflags"]);
	insertFlagsIfValid(leDmflags2, section["dmflags2"]);
	insertFlagsIfValid(leZandronumDmflags, section["zandronumDmflags"]);
	insertFlagsIfValid(leCompatflags, section["compatflags"]);
	insertFlagsIfValid(leZandronumCompatflags, section["zandronumCompatflags"]);
	insertFlagsIfValid(leLMSAllowedWeapons, section["lmsallowedweapons"], DEFAULT_LMSALLOWEDWEAPONS);
	insertFlagsIfValid(leLMSSpectatorSettings, section["lmsspectatorsettings"], DEFAULT_LMSSPECTATORSETTINGS);
	propagateFlagsInputsChanges();
	// End of old config support.

	FlagsId flagsId(this);
	flagsId.load(section);
	applyWidgetsChange();

	IniVariable varKillMonstersPercentage = section["killmonsters_percentage"];
	if (!varKillMonstersPercentage.value().isNull())
	{
		spinMonsterKillPercentage->setValue(varKillMonstersPercentage);
	}
	IniVariable varForceInactivePlayersSpectatingMins = section["force_inactive_players_spectating_mins"];
	if (!varForceInactivePlayersSpectatingMins.value().isNull())
	{
		spinForceInactivePlayersSpectatingMins->setValue(varForceInactivePlayersSpectatingMins);
	}
	IniVariable varMonstersDamageFactor = section["monsters_damage_factor"];
	if (!varMonstersDamageFactor.value().isNull())
	{
		spinMonstersDamageFactor->setValue(varMonstersDamageFactor);
	}

	if (section.hasSetting("falling_damage_type"))
		cboFallingDamage->setCurrentIndex(section["falling_damage_type"]);
	if (section.hasSetting("jump_ability"))
		cboJumping->setCurrentIndex(section["jump_ability"]);
	if (section.hasSetting("crouch_ability"))
		cboCrouching->setCurrentIndex(section["crouch_ability"]);
	setPlayerBlock(static_cast<PlayerBlock>(section.value("player_block", PB_NotSet).toInt()));
	setLevelExit(static_cast<LevelExit>(section.value("level_exit", EXIT_NotSet).toInt()));

	cbDefaultDmflags->setChecked((bool)section["defaultdmflags"]);

	return votingPage->loadConfig(ini);
}

void FlagsPage::propagateFlagsInputsChanges()
{
	if (d->flagsController != NULL)
		d->flagsController->convertNumericalToWidgets();
}

bool FlagsPage::saveConfig(Ini& ini)
{
	IniSection section = ini.section("dmflags");

	// Remove obsolete settings that were created by old method of saving
	// dmflags. That way the subsequent loads of this file will not trigger
	// the backward-compatibility fallbacks.
	QStringList oldSettings;
	oldSettings << "dmflags" << "dmflags2" << "zandronumDmflags" << "compatflags"
		<< "zandronumCompatflags" << "lmsallowedweapons" << "lmsspectatorsettings";
	foreach (const QString &oldSetting, oldSettings)
	{
		section.deleteSetting(oldSetting);
	}

	FlagsId flagsId(this);
	flagsId.save(section);

	section["gameversion"] = cboGameVersion->itemData(cboGameVersion->currentIndex()).toInt();
	section["defaultdmflags"] = cbDefaultDmflags->isChecked();
	section["falling_damage_type"] = cboFallingDamage->currentIndex();
	section["jump_ability"] = cboJumping->currentIndex();
	section["crouch_ability"] = cboCrouching->currentIndex();
	if (playerBlock() != PB_NotSet)
		section["player_block"] = playerBlock();
	if (levelExit() != EXIT_NotSet)
		section["level_exit"] = levelExit();

	section["killmonsters_percentage"] = spinMonsterKillPercentage->value();
	section["force_inactive_players_spectating_mins"] =
		spinForceInactivePlayersSpectatingMins->value();
	section["monsters_damage_factor"] =
		static_cast<float>(spinMonstersDamageFactor->value());

	return votingPage->saveConfig(ini);
}

void FlagsPage::applyGameVersion()
{
	setGameVersion(static_cast<GameVersion>(
		cboGameVersion->itemData(cboGameVersion->currentIndex()).toInt()));
}

void FlagsPage::loadGameVersion(GameVersion version)
{
	int index = cboGameVersion->findData(version);
	if (index < 0)
	{
		gLog << tr("Unknown Zandronum version in the config. Reverting to default.");
		version = DEFAULT_GAME_VERSION;
		index = cboGameVersion->findData(version);
		if (index < 0)
		{
			gLog << QString("Zandronum: FlagsPage::loadGameVersion() - oops, "
				"a bug!, GameVersion = %1") .arg(version);
			return;
		}
	}
	setGameVersion(version);
}

void FlagsPage::setGameVersion(GameVersion version)
{
	cboGameVersion->blockSignals(true);
	int index = cboGameVersion->findData(version);
	if (index >= 0)
		cboGameVersion->setCurrentIndex(index);
	cboGameVersion->blockSignals(false);
	if (d->flagsController != NULL)
	{
		d->flagsController->setVisible(false);
	}
	switch (version)
	{
	default:
		gLog << tr("Tried to set unknown Zandronum version. Reverting to default.");
		// intentional fall-through
	case GV_Zandronum2:
		d->flagsController = QSharedPointer<FlagsPageValueController>(
			new Zandronum2::FlagsPageValueController(this));
		break;
	case GV_Zandronum3:
		d->flagsController = QSharedPointer<FlagsPageValueController>(
			new Zandronum3::FlagsPageValueController(this));
		break;
	}
	d->flagsController->setVisible(true);
	d->flagsController->convertWidgetsToNumerical();
}

FlagsPage::PlayerBlock FlagsPage::playerBlock() const
{
	if (rbPlayersCanWalkThroughEachOther->isChecked())
		return PB_Noclip;
	if (rbAlliesCanWalkThroughEachOther->isChecked())
		return PB_AllyNoclip;
	if (rbPlayersBlockEachOtherNormally->isChecked())
		return PB_Block;
	return PB_NotSet;
}

void FlagsPage::setPlayerBlock(PlayerBlock playerBlock)
{
	switch (playerBlock)
	{
	case PB_NotSet:
		// do nothing
		break;
	case PB_Noclip:
		rbPlayersCanWalkThroughEachOther->setChecked(true);
		break;
	case PB_AllyNoclip:
		rbAlliesCanWalkThroughEachOther->setChecked(true);
		break;
	case PB_Block:
		rbPlayersBlockEachOtherNormally->setChecked(true);
		break;
	default:
		qDebug() << "FlagsPage::setPlayerBlock - unhandled PlayerBlock " << playerBlock;
		break;
	}
}

FlagsPage::LevelExit FlagsPage::levelExit() const
{
	if (rbContinueToTheNextMap->isChecked())
		return EXIT_NextMap;
	if (rbRestartTheCurrentLevel->isChecked())
		return EXIT_RestartMap;
	if (rbKillThePlayer->isChecked())
		return EXIT_KillPlayer;
	return EXIT_NotSet;
}

void FlagsPage::setLevelExit(LevelExit levelExit)
{
	switch (levelExit)
	{
	case EXIT_NotSet:
		// do nothing
		break;
	case EXIT_NextMap:
		rbContinueToTheNextMap->setChecked(true);
		break;
	case EXIT_RestartMap:
		rbRestartTheCurrentLevel->setChecked(true);
		break;
	case EXIT_KillPlayer:
		rbKillThePlayer->setChecked(true);
		break;
	default:
		qDebug() << "FlagsPage::setLevelExit - unhandled LevelExit " << levelExit;
		break;
	}
}
