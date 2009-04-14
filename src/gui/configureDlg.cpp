#include "gui/configureDlg.h"
#include <Qt>

ConfigureDlg::ConfigureDlg(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
	initOptionsList();

	connect(twOptionsList, SIGNAL( clicked(const QModelIndex&) ), this, SLOT( optionListClicked(const QModelIndex&) ) );
}

ConfigureDlg::~ConfigureDlg()
{
	for(int i = 0; i < engineConfigList.count(); ++i)
	{
		delete engineConfigList[i];
	}
}
/////////////////////////////////////////////////////////
void ConfigureDlg::initOptionsList()
{
	QTreeWidgetItem* engines = new QTreeWidgetItem(twOptionsList);
	engines->setText(0, tr("Engines"));
	QTreeWidgetItem* engineSkullTag = new QTreeWidgetItem(engines);
	engineSkullTag->setText(0, tr("SkullTag"));
	QTreeWidgetItem* blablabla = new QTreeWidgetItem(twOptionsList);
}
/////////////////////////////////////////////////////////
void ConfigureDlg::addEngineConfiguration(EngineConfiguration* ec)
{
	ec->confBox->setAttribute(Qt::WA_DeleteOnClose, true);
	engineConfigList.push_back(ec);
}
/////////////////////////////////////////////////////////
void ConfigureDlg::optionListClicked(const QModelIndex& index)
{
	QString str = index.data().toString();

	printf("clicked %s\n", str.toAscii().constData());
	Qt::CaseSensitivity cs = Qt::CaseInsensitive;

	for(int i = 0; i < engineConfigList.count(); ++i)
	{
		EngineConfiguration* ec = engineConfigList[i];
		if (str.compare(ec->engineName, cs) == 0 && ec->confBox != NULL)
		{
			ec->confBox->show();
		}
	}
}
