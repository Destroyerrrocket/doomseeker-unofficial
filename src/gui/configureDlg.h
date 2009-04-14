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
		QStandardItem*					hider;

		QStandardItem*					enginesRoot;
		QList<EngineConfiguration*>		engineConfigList;
		QWidget*						currentlyDisplayedCfgBox;

		void 		initOptionsList();
		void		saveSettings();

		void 					showConfigurationBox(QWidget*);
		EngineConfiguration*	findEngineConfiguration(const QStandardItem*);
		bool					isEngineConfiguration(const QStandardItem*);


	public:
		ConfigureDlg(Config* mainCfg, QWidget* parent = NULL);
		~ConfigureDlg();

		void addEngineConfiguration(EngineConfiguration*);

	public slots:
		void optionListClicked(const QModelIndex&);
		void btnClicked(QAbstractButton *button);
};

#endif
