#ifndef __ENGINE_CONFIG_BASE_H_
#define __ENGINE_CONFIG_BASE_H_

#include "sdeapi/config.hpp"
#include <QGroupBox>
#include <QStandardItem>


// Base class for configuration group boxes.
// This is the thing selected from the tree view on the left
// of configuration dialog.
class ConfigurationBaseBox : public QGroupBox
{
	Q_OBJECT;

	protected:
		Config* 	config;
		bool		bAllowSave;

		virtual void saveSettings()=0;

	public:
		ConfigurationBaseBox(Config* cfg, QWidget* parent = NULL) : QGroupBox(parent)
		{
			bAllowSave = false;
			config = cfg;
			hide();
		}

		virtual ~ConfigurationBaseBox() {}

		void setAllowSave(bool b)
		{
			bAllowSave = b;
		}

		// These shouldn't execute Config::readConfig() and Config::saveConfig()
		// methods. They're here to read settings from and write them to controls.
		virtual void readSettings()=0;
		bool save()
		{
			if (bAllowSave)
			{
				saveSettings();
				return true;
			}
			else
			{
				return false;
			}
		}

};

// Contains config info for
// particular config box
struct ConfigurationBoxInfo
{
	public:
		ConfigurationBoxInfo()
		{
			confBox = NULL;
			itemOnTheList = NULL;
		}

		~ConfigurationBoxInfo()
		{
			if (confBox != NULL)
			{
				delete confBox;
			}
		}
		// Name displayed on the engines list.
		QString 				boxName;

		// Index, used by ConfigureDlg class to find.
		QStandardItem*			itemOnTheList;

		ConfigurationBaseBox*	confBox;
};



#endif
