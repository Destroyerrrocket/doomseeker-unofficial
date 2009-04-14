#ifndef __CONFIGUREDIALOG_H_
#define __CONFIGUREDIALOG_H_

#include "ui_configureDlg.h"
#include "gui/engineConfigBase.h"

class ConfigureDlg : public QDialog, private Ui::ConfigureDlg
{
	Q_OBJECT

	private:
		QModelIndex						hider;
		QModelIndex						enginesRoot;
		QList<EngineConfiguration*>		engineConfigList;
		QWidget*						currentlyDisplayedCfgBox;

		void 		initOptionsList();

		void 		showConfigurationBox(QWidget*);
		QWidget*	findEngineConfigurationBox(const QModelIndex&);
		bool		isEngineConfiguration(const QModelIndex&);

	public:
		ConfigureDlg(QWidget* parent = NULL);
		~ConfigureDlg();

		void addEngineConfiguration(EngineConfiguration*);

	public slots:
		void optionListClicked(const QModelIndex&);
};

#endif
