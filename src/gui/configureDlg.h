#ifndef __CONFIGUREDIALOG_H_
#define __CONFIGUREDIALOG_H_

#include "gui/configBase.h"
#include "sdeapi/config.hpp"
#include "ui_configureDlg.h"

class ConfigureDlg : public QDialog, private Ui::ConfigureDlg
{
	Q_OBJECT

	private:
		Config*		mainConfig;
		// hider for debug purposes, if clicked
		// hides current box and shows nothing
		QStandardItem*					hider;

		QStandardItem*					enginesRoot;
		QList<ConfigurationBoxInfo*>	configBoxesList;
		QWidget*						currentlyDisplayedCfgBox;

		void 		initOptionsList();
		void		saveSettings();

		/**
		 * Passing NULL as rootItem will add the item to the tree view's root.
		 * Passing <0 to pos parameter will use appendRow method.
		 *
		 * Will return false and do nothing
		 * if the ConfigurationBoxInfo is not properly filled
		 * or if itemOnTheList member is not NULL.
		 */
		bool 					addConfigurationBox(QStandardItem* rootItem, ConfigurationBoxInfo*, int pos = -1);

		void 					showConfigurationBox(QWidget*);
		ConfigurationBoxInfo*	findConfigurationBoxInfo(const QStandardItem*);



	public:
		ConfigureDlg(Config* mainCfg, QWidget* parent = NULL);
		~ConfigureDlg();

		bool addEngineConfiguration(ConfigurationBoxInfo*);

	public slots:
		void optionListClicked(const QModelIndex&);
		void btnClicked(QAbstractButton *button);
};

#endif
