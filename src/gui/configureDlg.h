#ifndef __CONFIGUREDIALOG_H_
#define __CONFIGUREDIALOG_H_

#include "ui_configureDlg.h"
#include "gui/configBase.h"
#include "sdeapi/config.hpp"

class ConfigureDlg : public QDialog, private Ui::ConfigureDlg
{
	Q_OBJECT

	private:
		Config*		mainConfig;
		// hider for debug purposes, if clicked
		// hides current box and shows nothing
		QModelIndex						hider;

		QModelIndex						enginesRoot;
		QList<EngineConfiguration*>		engineConfigList;
		QWidget*						currentlyDisplayedCfgBox;

		void 		initOptionsList();
		void		saveSettings();

		void 					showConfigurationBox(QWidget*);
		EngineConfiguration*	findEngineConfigurationBox(const QModelIndex&);
		bool					isEngineConfiguration(const QModelIndex&);


	public:
		ConfigureDlg(Config* mainCfg, QWidget* parent = NULL);
		~ConfigureDlg();

		void addEngineConfiguration(EngineConfiguration*);

	public slots:
		void optionListClicked(const QModelIndex&);
		void btnOkClicked();
		void btnApplyClicked();
};

#endif
