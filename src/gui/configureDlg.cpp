#include "gui/configureDlg.h"
#include <Qt>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>

ConfigureDlg::ConfigureDlg(Config* mainCfg, QWidget* parent) : QDialog(parent)
{
	mainConfig = mainCfg;
	mainConfig->readConfig();
	setupUi(this);
	initOptionsList();

	currentlyDisplayedCfgBox = NULL;
	connect(tvOptionsList, SIGNAL( clicked(const QModelIndex&) ), this, SLOT( optionListClicked(const QModelIndex&) ) );
	connect(btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ));
	connect(btnOk, SIGNAL( clicked() ), this, SLOT( btnOkClicked() ));
	connect(btnApply, SIGNAL( clicked() ), this, SLOT( btnApplyClicked() ));
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
	QStandardItemModel* model = new QStandardItemModel(this);
	QStandardItem* root1;

	root1 = new QStandardItem("Engines");
	model->appendRow(root1);
	enginesRoot = root1;

	root1 = new QStandardItem("<HIDE>");
	model->appendRow(root1);
	hider = root1;

	tvOptionsList->setModel(model);
}

void ConfigureDlg::saveSettings()
{
	qDebug() << "Saving settings:";
	// Iterate through every engine and execute it's saving method
	for (int i = 0; i < engineConfigList.count(); ++i)
	{
		qDebug() << "Engine:" << engineConfigList[i]->engineName;
		engineConfigList[i]->confBox->saveSettings();
	}
	mainConfig->saveConfig();
	qDebug() << "Saving completed!";
}
/////////////////////////////////////////////////////////
// This will hide currently displayed box if NULL is passed
// as w argument.
void ConfigureDlg::showConfigurationBox(QWidget* w)
{
	if (currentlyDisplayedCfgBox != NULL)
	{
		currentlyDisplayedCfgBox->hide();
	}
	currentlyDisplayedCfgBox = w;

	if (w != NULL)
	{
		int width = this->width();
		int height = btnApply->y() - tvOptionsList->y() - 10;
		int posX = tvOptionsList->x() + tvOptionsList->width() + 5;
		int posY = tvOptionsList->y();
		width -= posX + 10;

		w->move(posX, posY);
		w->resize(width, height);
		w->show();
	}
}

EngineConfiguration* ConfigureDlg::findEngineConfiguration(const QStandardItem* item)
{
	// Cycle through known engines
	for(int i = 0; i < engineConfigList.count(); ++i)
	{
		EngineConfiguration* ec = engineConfigList[i];
		if (item == ec->itemOnTheList && ec->confBox != NULL)
		{
			return ec;
		}
	}

	return NULL;
}

// Returns true if item is a child of Engines root item
bool ConfigureDlg::isEngineConfiguration(const QStandardItem* item)
{
	QStandardItem* parent = item->parent();

	while (parent != NULL)
	{
		if (parent == enginesRoot)
		{
			return true;
		}
		parent = parent->parent();
	}
	return false;
}
/////////////////////////////////////////////////////////
void ConfigureDlg::addEngineConfiguration(EngineConfiguration* ec)
{
	if (ec != NULL)
	{
		QStandardItemModel* model = (QStandardItemModel*)tvOptionsList->model();
		QStandardItem* item = new QStandardItem(ec->engineName);

		enginesRoot->appendRow(item);

		ec->confBox->setAttribute(Qt::WA_DeleteOnClose, true);
		ec->itemOnTheList = item;
		engineConfigList.push_back(ec);
	}
}
/////////////////////////////////////////////////////////
void ConfigureDlg::optionListClicked(const QModelIndex& index)
{
	QString str = index.data().toString();
	QStandardItemModel* model = static_cast<QStandardItemModel*>(tvOptionsList->model());
	QStandardItem* item = model->itemFromIndex(index);

	if (item == hider)
	{
		showConfigurationBox(NULL);
		return;
	}

	QWidget* newWidget = NULL;

	if (isEngineConfiguration(item))
	{
		EngineConfiguration *ec = findEngineConfiguration(item);
		ec->confBox->readSettings();
		newWidget = ec->confBox;
	}

	// Something with sense was selected, display this something
	// and hide previous box.
	if (newWidget != NULL)
	{
		showConfigurationBox(newWidget);
	}
}

void ConfigureDlg::btnOkClicked()
{
	this->saveSettings();
	this->accept();
}

void ConfigureDlg::btnApplyClicked()
{
	this->saveSettings();
}
