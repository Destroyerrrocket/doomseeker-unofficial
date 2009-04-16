#include "gui/configureDlg.h"
#include <Qt>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QAbstractButton>

ConfigureDlg::ConfigureDlg(Config* mainCfg, QWidget* parent) : QDialog(parent)
{
	mainConfig = mainCfg;
	mainConfig->readConfig();
	setupUi(this);
	initOptionsList();

	tvOptionsList->setHeaderHidden(true);

	currentlyDisplayedCfgBox = NULL;
	connect(tvOptionsList, SIGNAL( clicked(const QModelIndex&) ), this, SLOT( optionListClicked(const QModelIndex&) ) );
	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT ( btnClicked(QAbstractButton *) ));
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
		mainPanel->layout()->addWidget(w);
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

void ConfigureDlg::btnClicked(QAbstractButton *button)
{
	// Figure out what button we pressed and perform its action.
	switch(buttonBox->standardButton(button))
	{
		default:
			break;
		case QDialogButtonBox::Ok: // Also does the same as Apply
			this->accept();
		case QDialogButtonBox::Apply:
			this->saveSettings();
			break;
		case QDialogButtonBox::Cancel:
			this->reject();
			break;
	}
}
