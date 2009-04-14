#include "gui/engineSkulltagConfig.h"

EngineSkulltagConfigBox::EngineSkulltagConfigBox(QWidget* parent) : EngineConfigurationBaseBox(parent)
{
	setupUi(this);
}
///////////////////////////////////////////////////
EngineConfiguration* EngineSkulltagConfigBox::engineConfiguration(QWidget* parent)
{
	EngineConfiguration* ec = new EngineConfiguration();
	ec->confBox = new EngineSkulltagConfigBox(parent);
	ec->engineName = "SkullTag";
	return ec;
}
///////////////////////////////////////////////////
void EngineSkulltagConfigBox::readConfig()
{
}

void EngineSkulltagConfigBox::saveConfig()
{
}
