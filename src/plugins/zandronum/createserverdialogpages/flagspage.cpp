#include "flagspage.h"

#include <QDebug>
#include <QValidator>
#include <ini/ini.h>
#include <ini/inisection.h>

#include "createserverdialogpages/flagspagevaluecontroller.h"
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
			unsigned val = input.toUInt(&bOk);


			return bOk ? QValidator::Acceptable : QValidator::Invalid;
		}
};
////////////////////////////////////////////////////////////////////////////////
class FlagsPage::PrivData
{
	public:
		DmflagsValidator validator;
};

////////////////////////////////////////////////////////////////////////////////
FlagsPage::FlagsPage(CreateServerDialog* pParentDialog)
: CreateServerDialogPage(pParentDialog, tr("Flags"))
{
	setupUi(this);

	d = new PrivData();

	// The validator makes sure that the line edit will accept only
	// 32-bit unsigned values.
	leDmflags->setValidator(&d->validator);
	leDmflags2->setValidator(&d->validator);
	leDmflags3->setValidator(&d->validator);
	leCompatflags->setValidator(&d->validator);
	leCompatflags2->setValidator(&d->validator);
	leLMSAllowedWeapons->setValidator(&d->validator);
	leLMSSpectatorSettings->setValidator(&d->validator);

	// Init values for widgets.
	cboFallingDamage->insertItem(FDT_None, tr("None"));
	cboFallingDamage->insertItem(FDT_Old, tr("Old (ZDoom)"));
	cboFallingDamage->insertItem(FDT_Hexen, tr("Hexen"));
	cboFallingDamage->insertItem(FDT_Strife, tr("Strife"));
	cboFallingDamage->setCurrentIndex(FDT_None);

	initJumpCrouchComboBoxes(cboJumping);
	initJumpCrouchComboBoxes(cboCrouching);

	// Widget states
	spinMonsterKillPercentage->setEnabled(false);
}

FlagsPage::~FlagsPage()
{
	delete d;
}

void FlagsPage::applyWidgetsChange()
{
	FlagsPageValueController controller(this);
	controller.convertWidgetsToNumerical();
}

QStringList FlagsPage::generateGameRunParameters()
{
	QStringList params;

	params << "+dmflags" << leDmflags->text();
	params << "+dmflags2" << leDmflags2->text();
	params << "+dmflags3" << leDmflags3->text();
	params << "+compatflags" << leCompatflags->text();
	params << "+compatflags2" << leCompatflags2->text();
	params << "+lmsallowedweapons" << leLMSAllowedWeapons->text();
	params << "+lmsspectatorsettings" << leLMSSpectatorSettings->text();

	unsigned dmflags2 = leDmflags2->text().toUInt();
	if (dmflags2 & ZandronumGameInfo::DF2_KILL_MONSTERS)
	{
		params << "+sv_killallmonsters_percentage"
			<< QString::number(spinMonsterKillPercentage->value());
	}
	params << "+sv_afk2spec" <<
		QString::number(spinForceInactivePlayersSpectatingMins->value());
	params << "+sv_coop_damagefactor" <<
		QString::number(spinMonstersDamageFactor->value());

	params << "+sv_defaultdmflags" << (cbDefaultDmflags->isChecked() ? "1" : "0");

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

	quint32 oldDmflags = 0, oldDmflags2 = 0, oldCompatflags = 0;
	loadOldDmflags(section, oldDmflags, oldDmflags2, oldCompatflags);

	insertFlagsIfValid(leDmflags, section["dmflags"], oldDmflags);
	insertFlagsIfValid(leDmflags2, section["dmflags2"], oldDmflags2);
	insertFlagsIfValid(leDmflags3, section["dmflags3"]);
	insertFlagsIfValid(leCompatflags, section["compatflags"], oldCompatflags);
	insertFlagsIfValid(leCompatflags2, section["compatflags2"]);
	insertFlagsIfValid(leLMSAllowedWeapons, section["lmsallowedweapons"], DEFAULT_LMSALLOWEDWEAPONS);
	insertFlagsIfValid(leLMSSpectatorSettings, section["lmsspectatorsettings"], DEFAULT_LMSSPECTATORSETTINGS);

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

	cbDefaultDmflags->setChecked((bool)section["defaultdmflags"]);

	propagateFlagsInputsChanges();

	return true;
}

void FlagsPage::loadOldDmflags(IniSection &section, quint32 &dmflags, quint32 &dmflags2, quint32 &compatFlags) const
{
	// For a smooth transition read in old dmflag values.

	const QList<DMFlagsSection>* dmflagsList = ZandronumGameInfo::dmFlags();

	foreach(const DMFlagsSection &flags, *dmflagsList)
	{
		quint32 *flagsSet;
		if(flags.name() == "DMFlags")
			flagsSet = &dmflags;
		else if(flags.name() == "DMFlags2")
			flagsSet = &dmflags2;
		else if(flags.name() == "Compat. flags")
			flagsSet = &compatFlags;
		else
			continue;

		for (int i = 0; i < flags.count(); ++i)
		{
			const DMFlag& flag = flags[i];
			QRegExp re("[^a-zA-Z]");
			QString settingName = flags.name() + flag.name();
			settingName.remove(re);

			IniVariable var = section.retrieveSetting(settingName);
			if(!var.isNull())
			{
				if((bool)var)
					*flagsSet |= 1<<flag.value();

				// We no longer need to keep the setting around
				section.deleteSetting(settingName);
			}
		}
	}
}

void FlagsPage::propagateFlagsInputsChanges()
{
	FlagsPageValueController controller(this);
	controller.convertNumericalToWidgets();
}

bool FlagsPage::saveConfig(Ini& ini)
{
	IniSection section = ini.section("dmflags");

	section["dmflags"] = leDmflags->text();
	section["dmflags2"] = leDmflags2->text();
	section["dmflags3"] = leDmflags3->text();
	section["compatflags"] = leCompatflags->text();
	section["compatflags2"] = leCompatflags2->text();
	section["lmsallowedweapons"] = leLMSAllowedWeapons->text();
	section["lmsspectatorsettings"] = leLMSSpectatorSettings->text();
	section["defaultdmflags"] = cbDefaultDmflags->isChecked();

	unsigned dmflags2 = leDmflags2->text().toUInt();
	if (dmflags2 & ZandronumGameInfo::DF2_KILL_MONSTERS)
	{
		section["killmonsters_percentage"] = spinMonsterKillPercentage->value();
	}
	section["force_inactive_players_spectating_mins"] =
		spinForceInactivePlayersSpectatingMins->value();
	section["monsters_damage_factor"] =
		(float)spinMonstersDamageFactor->value();

	return true;
}

