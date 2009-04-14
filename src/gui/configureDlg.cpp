#include "gui/configureDlg.h"
#include <Qt>
#include <QScrollBar>

ConfigureDlg::ConfigureDlg(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
	initOptionsList();

	currentlyDisplayedCfgBox = NULL;
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
	enginesRoot = engines;
	QTreeWidgetItem* engineSkullTag = new QTreeWidgetItem(engines);
	engineSkullTag->setText(0, tr("SkullTag"));
	QTreeWidgetItem* fakeSkullTag = new QTreeWidgetItem(twOptionsList);
	fakeSkullTag->setText(0, tr("SkullTag"));
}
/////////////////////////////////////////////////////////
void ConfigureDlg::showConfigurationBox(QWidget* w)
{
	int width = this->width();
	int posX = twOptionsList->x() + twOptionsList->width() + 5;
	int posY = twOptionsList->y();
	width -= posX + 10;

	w->move(posX, posY);
	//w->resize(width, posY);
	w->show();
}

QWidget* ConfigureDlg::findEngineConfigurationBox(const QTreeWidgetItem& item)
{
	QString str = item.text(0);
	Qt::CaseSensitivity cs = Qt::CaseInsensitive;

	// Cycle through known engines
	for(int i = 0; i < engineConfigList.count(); ++i)
	{
		EngineConfiguration* ec = engineConfigList[i];
		if (str.compare(ec->engineName, cs) == 0 && ec->confBox != NULL && isEngineConfiguration(item))
		{
			return ec->confBox;
		}
	}

	return NULL;
}

// Returns true if item is a child of Engines root item
bool ConfigureDlg::isEngineConfiguration(const QTreeWidgetItem& item)
{
	QTreeWidgetItem* parent = item.parent();
	Qt::CaseSensitivity cs = Qt::CaseInsensitive;

	while (parent != NULL)
	{
		if (parent == enginesRoot)
		{
			return true;
		}
	}
	return false;
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
	QTreeWidgetItem* item = (QTreeWidgetItem*)index.model();
	QString str = item->text(0);

	printf("clicked %s\n", str.toAscii().constData());

	QWidget* newWidget = NULL;

	newWidget = findEngineConfigurationBox(*item);

	// Something with sense was selected, display this something
	// and hide previous box.
	if (newWidget != NULL)
	{
		if (currentlyDisplayedCfgBox != NULL)
		{
			currentlyDisplayedCfgBox->hide();
		}
		showConfigurationBox(newWidget);
		currentlyDisplayedCfgBox = newWidget;
	}
}
