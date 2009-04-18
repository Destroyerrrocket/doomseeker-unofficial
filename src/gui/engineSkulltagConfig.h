#ifndef __ENGINE_SKULLTAG_CONFIG_H_
#define __ENGINE_SKULLTAG_CONFIG_H_

#include "gui/configBase.h"
#include "ui_engineSkulltagConfig.h"

class EngineSkulltagConfigBox : public ConfigurationBaseBox, private Ui::EngineSkulltagConfigBox
{
	Q_OBJECT

	protected:
		EngineSkulltagConfigBox(Config* cfg, QWidget* parent = NULL);
		void saveSettings();

	public:
		static ConfigurationBoxInfo* createStructure(Config* cfg, QWidget* parent = NULL);

		void readSettings();


	public slots:
		void btnBrowseBinaryClicked();
};

#endif
