#ifndef __ENGINE_CONFIG_BASE_H_
#define __ENGINE_CONFIG_BASE_H_

#include <QGroupBox>
#include <QModelIndex>

class EngineConfigurationBaseBox;

struct EngineConfiguration
{
	public:
		QString 					engineName;
		QModelIndex					indexOnTheList;
		EngineConfigurationBaseBox*	confBox;
};

class EngineConfigurationBaseBox : public QGroupBox
{
	Q_OBJECT;

	public:
		EngineConfigurationBaseBox(QWidget* parent = NULL) : QGroupBox(parent)
		{
			hide();
		}

		virtual ~EngineConfigurationBaseBox() {}

		virtual void readConfig()=0;
		virtual void saveConfig()=0;

};

#endif
