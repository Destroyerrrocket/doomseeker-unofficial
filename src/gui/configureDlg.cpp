#include "gui/configureDlg.h"
#include <Qt>
#include <QStandardItemModel>
#include <QStandardItem>

ConfigureDlg::ConfigureDlg(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
	initOptionsList();

	currentlyDisplayedCfgBox = NULL;
	connect(tvOptionsList, SIGNAL( clicked(const QModelIndex&) ), this, SLOT( optionListClicked(const QModelIndex&) ) );
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
	enginesRoot = model->indexFromItem(root1);

	root1 = new QStandardItem("<HIDE>");
	model->appendRow(root1);
	hider = model->indexFromItem(root1);

	tvOptionsList->setModel(model);
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
		int posX = tvOptionsList->x() + tvOptionsList->width() + 5;
		int posY = tvOptionsList->y();
		width -= posX + 10;

		w->move(posX, posY);
		w->resize(width, tvOptionsList->height());
		w->show();
	}
}

QWidget* ConfigureDlg::findEngineConfigurationBox(const QModelIndex& index)
{
	// Cycle through known engines
	for(int i = 0; i < engineConfigList.count(); ++i)
	{
		EngineConfiguration* ec = engineConfigList[i];
		if (index == ec->indexOnTheList && ec->confBox != NULL && isEngineConfiguration(index))
		{
			return ec->confBox;
		}
	}

	return NULL;
}

// Returns true if item is a child of Engines root item
bool ConfigureDlg::isEngineConfiguration(const QModelIndex& index)
{

	QModelIndex parent = index.parent();

	while (parent.isValid())
	{
		if (parent == enginesRoot)
		{
			return true;
		}
		parent = parent.parent();
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
		QStandardItem* root = model->itemFromIndex(enginesRoot);

		root->appendRow(item);

		ec->confBox->setAttribute(Qt::WA_DeleteOnClose, true);
		ec->indexOnTheList = model->indexFromItem(item);
		engineConfigList.push_back(ec);
	}
}
/////////////////////////////////////////////////////////
void ConfigureDlg::optionListClicked(const QModelIndex& index)
{
	QString str = index.data().toString();

	if (index == hider)
	{
		showConfigurationBox(NULL);
	}

	QWidget* newWidget = NULL;

	newWidget = findEngineConfigurationBox(index);

	// Something with sense was selected, display this something
	// and hide previous box.
	if (newWidget != NULL)
	{
		showConfigurationBox(newWidget);
	}
}
