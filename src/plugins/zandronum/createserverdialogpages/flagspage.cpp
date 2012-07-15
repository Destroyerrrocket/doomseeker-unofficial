#include "flagspage.h"

#include <QDebug>
#include <QValidator>
#include <ini/ini.h>
#include <ini/inisection.h>

#include "createserverdialogpages/flagspagevaluecontroller.h"
#include "zandronumgameinfo.h"

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

	return params;
}

void FlagsPage::initJumpCrouchComboBoxes(QComboBox* pComboBox)
{
	pComboBox->insertItem(JCA_Default, tr("Default"));
	pComboBox->insertItem(JCA_No, tr("No"));
	pComboBox->insertItem(JCA_Yes, tr("Yes"));
}

void FlagsPage::insertFlagsIfValid(QLineEdit* dst, QString flags)
{
	int dummy;
	if (d->validator.validate(flags, dummy) == QValidator::Acceptable)
	{
		dst->setText(flags);
	}
}

bool FlagsPage::loadConfig(Ini& ini)
{
	IniSection section = ini.section("dmflags");

	insertFlagsIfValid(leDmflags, section["dmflags"]);
	insertFlagsIfValid(leDmflags2, section["dmflags2"]);
	insertFlagsIfValid(leDmflags3, section["dmflags3"]);
	insertFlagsIfValid(leCompatflags, section["compatflags"]);
	insertFlagsIfValid(leCompatflags2, section["compatflags2"]);
	insertFlagsIfValid(leLMSAllowedWeapons, section["lmsallowedweapons"]);
	insertFlagsIfValid(leLMSSpectatorSettings, section["lmsspectatorsettings"]);

	IniVariable varKillMonstersPercentage = section["killmonsters_percentage"];
	if (!varKillMonstersPercentage.value().isNull())
	{
		spinMonsterKillPercentage->setValue(varKillMonstersPercentage);
	}

	propagateFlagsInputsChanges();

	return true;
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

	unsigned dmflags2 = leDmflags2->text().toUInt();
	if (dmflags2 & ZandronumGameInfo::DF2_KILL_MONSTERS)
	{
		section["killmonsters_percentage"] = spinMonsterKillPercentage->value();
	}

	return true;
}

