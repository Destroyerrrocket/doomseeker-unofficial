#ifndef __ENGINE_SKULLTAG_CONFIG_H_
#define __ENGINE_SKULLTAG_CONFIG_H_

#include "gui/engineConfigBase.h"
#include "ui_engineSkulltagConfig.h"

class EngineSkulltagConfigBox : public EngineConfigurationBaseBox, private Ui::EngineSkulltagConfigBox
{
	Q_OBJECT

	private:
		EngineSkulltagConfigBox(QWidget* parent = NULL);

	public:
		static EngineConfiguration* engineConfiguration(QWidget* parent = NULL);

		void readConfig();
		void saveConfig();
};

#endif
