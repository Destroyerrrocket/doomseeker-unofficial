//------------------------------------------------------------------------------
// cfgcustomservers.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "configuration/doomseekerconfig.h"
#include "cfgcustomservers.h"
#include "ui_cfgcustomservers.h"
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QUrl>

const // clear warnings
#include "unknownengine.xpm"

DClass<CFGCustomServers> : public Ui::CFGCustomServers
{
};

DPointered(CFGCustomServers)

CFGCustomServers::CFGCustomServers(QWidget *parent)
: ConfigPage(parent)
{
	d->setupUi(this);

	connect(d->btnAdd, SIGNAL( clicked() ), this, SLOT( add() ));
	connect(d->btnRemove, SIGNAL( clicked() ), this, SLOT( remove() ));
	connect(d->btnSetEngine, SIGNAL( clicked() ), this, SLOT( setEngine() ));

	prepareEnginesComboBox();
}

CFGCustomServers::~CFGCustomServers()
{
}

void CFGCustomServers::add()
{
	int pluginIndex = d->cboEngines->itemData(d->cboEngines->currentIndex()).toInt();
	const EnginePlugin* plugin = gPlugins->info(pluginIndex);

	QString engineName = d->cboEngines->itemText(d->cboEngines->currentIndex());

	add(engineName, "", plugin->data()->defaultServerPort);
}

void CFGCustomServers::add(const QString& engineName, const QString& host, unsigned short port)
{
	QList<QStandardItem* > record;

	QStandardItem* engineItem = new QStandardItem();
	setEngineOnItem(engineItem, engineName);

	QString portString = QString::number(port);

	record.append(engineItem);
	record.append(new QStandardItem(host));
	record.append(new QStandardItem(portString));

	model->appendRow(record);
	d->tvServers->resizeRowsToContents();
}

CFGCustomServers::CheckAndFixPorts CFGCustomServers::checkAndFixPorts(int firstRow, int lastRow)
{
	CheckAndFixPorts returnValue = AllOk;
	for (int rowIndex = firstRow; rowIndex <= lastRow; ++rowIndex)
	{
		if (!isPortCorrect(rowIndex))
		{
			returnValue = AtLeastOneFixed;

			setPortToDefault(rowIndex);
		}
	}

	return returnValue;
}

void CFGCustomServers::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
	const QString MESSAGE_TITLE = tr("Doomseeker - custom servers");

	int leftmostColumn = topLeft.column();
	int rightmostColumn = bottomRight.column();

	if ( isPortColumnWithingRange(leftmostColumn, rightmostColumn) )
	{
		switch ( checkAndFixPorts(topLeft.row(), bottomRight.row()) )
		{
			case AllOk:
				// Ignore
				break;

			case AtLeastOneFixed:
				QMessageBox::warning(this, MESSAGE_TITLE, tr("Port must be within range 1 - 65535"));
				break;

			default:
				QMessageBox::warning(this, MESSAGE_TITLE, tr("Unimplemented behavior!"));
				break;
		}
	}
}

const EnginePlugin* CFGCustomServers::getPluginInfoForRow(int rowIndex)
{
	QStandardItem* itemEngine = model->item(rowIndex, EngineColumnIndex);
	QString engineName = itemEngine->data().toString();
	int pluginIndex = gPlugins->pluginIndexFromName(engineName);
	return gPlugins->info(pluginIndex);
}

bool CFGCustomServers::isPortColumnWithingRange(int leftmostColumnIndex, int rightmostColumnIndex)
{
	return leftmostColumnIndex <= PortColumnIndex && rightmostColumnIndex >= PortColumnIndex;
}

bool CFGCustomServers::isPortCorrect(int rowIndex)
{
	const int MIN_PORT = 1;
	const int MAX_PORT = 65535;

	QStandardItem* item = model->item(rowIndex, PortColumnIndex);
	bool ok;
	int port = item->text().toInt(&ok);

	return ok && port >= MIN_PORT && port <= MAX_PORT;
}

void CFGCustomServers::prepareEnginesComboBox()
{
	d->cboEngines->clear();

	for (unsigned i = 0; i < gPlugins->numPlugins(); ++i)
	{
		const EnginePlugin* plugin = gPlugins->info(i);
		d->cboEngines->addItem(plugin->icon(), plugin->data()->name, i);
	}

	if (d->cboEngines->count() > 0)
	{
		d->cboEngines->setCurrentIndex(0);
	}
}

void CFGCustomServers::prepareTable()
{
	model = new QStandardItemModel(this);

	connect(model, SIGNAL( dataChanged(const QModelIndex&, const QModelIndex&) ), this, SLOT( dataChanged(const QModelIndex&, const QModelIndex&) ) );

	QStringList labels;
	labels << "" << tr("Host") << tr("Port");
	model->setHorizontalHeaderLabels(labels);

	d->tvServers->setModel(model);

	d->tvServers->setColumnWidth(0, 23);
	d->tvServers->setColumnWidth(1, 180);
	d->tvServers->setColumnWidth(2, 60);

	d->tvServers->horizontalHeader()->setHighlightSections(false);
#if QT_VERSION >= 0x050000
	d->tvServers->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
#else
	d->tvServers->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
#endif

	d->tvServers->verticalHeader()->hide();
}

void CFGCustomServers::readSettings()
{
	prepareTable();

	QList<CustomServerInfo> customServersList = gConfig.doomseeker.customServers.toList();
	QList<CustomServerInfo>::iterator it;
	for (it = customServersList.begin(); it != customServersList.end(); ++it)
	{
		add(it->engine, it->host, it->port);
	}
}

void CFGCustomServers::remove()
{
	QItemSelectionModel* selModel = d->tvServers->selectionModel();
	QModelIndexList indexList = selModel->selectedRows();
	selModel->clear();

	QList<QStandardItem*> itemList;
	for (int i = 0; i < indexList.count(); ++i)
	{
		itemList << model->item(indexList[i].row(), 0);
	}

	for (int i = 0; i < itemList.count(); ++i)
	{
		QModelIndex index = model->indexFromItem(itemList[i]);
		model->removeRow(index.row());
	}
}

void CFGCustomServers::saveSettings()
{
	gConfig.doomseeker.customServers = this->tableGetServers();
}

void CFGCustomServers::setEngine()
{
	QItemSelectionModel* sel = d->tvServers->selectionModel();
	QModelIndexList indexList = sel->selectedRows();

	QModelIndexList::iterator it;
	for (it = indexList.begin(); it != indexList.end(); ++it)
	{
		QStandardItem* item = model->itemFromIndex(*it);
		QString engineName = d->cboEngines->itemText(d->cboEngines->currentIndex());
		setEngineOnItem(item, engineName);
	}
}

void CFGCustomServers::setEngineOnItem(QStandardItem* item, const QString& engineName)
{
	int engineId = gPlugins->pluginIndexFromName(engineName);

	item->setData(engineName);
	item->setToolTip(engineName);
	if (engineId >= 0)
	{
		const EnginePlugin* plugin = gPlugins->info(engineId);
		item->setIcon(plugin->icon());
	}
	else
	{
		item->setIcon(QPixmap(unknownengine_xpm));
	}

	item->setEditable(false);
	item->setText("");
}

void CFGCustomServers::setPortToDefault(int rowIndex)
{
	const EnginePlugin* pluginInfo = getPluginInfoForRow(rowIndex);
	QString defaultPort = QString::number(pluginInfo->data()->defaultServerPort);

	QStandardItem* itemPort = model->item(rowIndex, PortColumnIndex);
	itemPort->setText(defaultPort);
}

QVector<CustomServerInfo> CFGCustomServers::tableGetServers()
{
	QVector<CustomServerInfo> servers;
	for (int i = 0; i < model->rowCount(); ++i)
	{
		CustomServerInfo customServer;

		QStandardItem* item = model->item(i, EngineColumnIndex);
		customServer.engine = item->data().toString();

		customServer.engineIndex = gPlugins->pluginIndexFromName(customServer.engine);

		item = model->item(i, AddressColumnIndex);
		customServer.host = item->text();

		item = model->item(i, PortColumnIndex);
		customServer.port = item->text().toUShort();

		servers << customServer;
	}

	return servers;
}
