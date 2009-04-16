#include "gui/engineSkulltagConfig.h"
#include <QFileDialog>

EngineSkulltagConfigBox::EngineSkulltagConfigBox(Config* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	connect(btnBrowseBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseBinaryClicked() ));
}
///////////////////////////////////////////////////
ConfigurationBoxInfo* EngineSkulltagConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->confBox = new EngineSkulltagConfigBox(cfg, parent);
	ec->boxName = "SkullTag";
	return ec;
}
///////////////////////////////////////////////////
void EngineSkulltagConfigBox::readSettings()
{
	QString str;
	SettingsData* setting;

	setting = config->setting("SkullTagBinaryPath");

	leBinaryPath->setText(setting->string());
}

void EngineSkulltagConfigBox::saveSettings()
{
	QString strVal;
	SettingsData* setting;

	strVal = leBinaryPath->text();
	setting = config->setting("SkullTagBinaryPath");
	setting->setValue(strVal);
}
////////////////////////////////////////////////////
void EngineSkulltagConfigBox::btnBrowseBinaryClicked()
{
	QString filter = tr("Binary files (*.exe);;Any files (*)");
	QString strFilepath = QFileDialog::getOpenFileName(this, tr("Doomseeker - choose SkullTag binary"), QString(), filter);
	leBinaryPath->setText(strFilepath);
}
