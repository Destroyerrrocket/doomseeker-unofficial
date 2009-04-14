#ifndef __CONFIGUREDIALOG_H_
#define __CONFIGUREDIALOG_H_

#include "ui_configureDlg.h"
#include "gui/engineConfigBase.h"

class ConfigureDlg : public QDialog, private Ui::ConfigureDlg
{
	Q_OBJECT

	private:
		QTreeWidgetItem*				enginesRoot;
		QList<EngineConfiguration*>		engineConfigList;
		QWidget*						currentlyDisplayedCfgBox;

		void 		initOptionsList();

		void 		showConfigurationBox(QWidget*);
		QWidget*	findEngineConfigurationBox(const QTreeWidgetItem&);
		bool		isEngineConfiguration(const QTreeWidgetItem&);

	public:
		ConfigureDlg(QWidget* parent = NULL);
		~ConfigureDlg();

		void addEngineConfiguration(EngineConfiguration*);

	public slots:
		void optionListClicked(const QModelIndex&);
};

#endif
