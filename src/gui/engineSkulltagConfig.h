#ifndef __ENGINE_SKULLTAG_CONFIG_H_
#define __ENGINE_SKULLTAG_CONFIG_H_

#include "gui/configBase.h"
#include "ui_engineSkulltagConfig.h"

class EngineSkulltagConfigBox : public ConfigurationBaseBox, private Ui::EngineSkulltagConfigBox
{
	Q_OBJECT

	private:
		EngineSkulltagConfigBox(Config* cfg, QWidget* parent = NULL);

	protected:
		void resizeEvent(QResizeEvent* event);

	public:
		static EngineConfiguration* createStructure(Config* cfg, QWidget* parent = NULL);

		void readSettings();
		void saveSettings();

	public slots:
		void btnBrowseBinaryClicked();
};

#endif
