#ifndef __CFG_FILE_PATHS_H_
#define __CFG_FILE_PATHS_H_

#include "gui/configBase.h"
#include "ui_cfgFilePaths.h"

class FilePathsConfigBox : public ConfigurationBaseBox, private Ui::FilePathsConfigBox
{
	Q_OBJECT

	protected:
		FilePathsConfigBox(Config*, QWidget* parent = 0);

	protected slots:
		void btnAddWadPath_Click();
		void btnRemoveWadPath_Click();

	public:
		static ConfigurationBoxInfo* createStructure(Config* cfg, QWidget* parent = NULL);

		void readSettings();
		void saveSettings();
};

#endif
