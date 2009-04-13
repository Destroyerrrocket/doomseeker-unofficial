#include "gui/configureDlg.h"
#include <Qt>

ConfigureDlg::ConfigureDlg(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
	initOptionsList();
}

void ConfigureDlg::initOptionsList()
{
	QTreeWidgetItem* engines = new QTreeWidgetItem(twOptionsList);
	engines->setText(0, tr("Engines"));
	QTreeWidgetItem* engineSkullTag = new QTreeWidgetItem(engines);
	engineSkullTag->setText(0, tr("SkullTag"));
	QTreeWidgetItem* blablabla = new QTreeWidgetItem(twOptionsList);
	blablabla->setText(0, tr("blah blah blah"));
}
