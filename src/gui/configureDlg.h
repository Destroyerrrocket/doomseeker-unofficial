#ifndef __CONFIGUREDIALOG_H_
#define __CONFIGUREDIALOG_H_

#include "ui_configureDlg.h"

struct EngineConfiguration
{
	QString 	engineName;
	QWidget*	confBox;
};

class ConfigureDlg : public QDialog, private Ui::ConfigureDlg
{
	Q_OBJECT

	private:
		QList<EngineConfiguration*>		engineConfigList;
		void initOptionsList();

	public:
		ConfigureDlg(QWidget* parent = 0);
		~ConfigureDlg();

		void addEngineConfiguration(EngineConfiguration*);

	public slots:
		void optionListClicked(const QModelIndex&);
};

#endif
