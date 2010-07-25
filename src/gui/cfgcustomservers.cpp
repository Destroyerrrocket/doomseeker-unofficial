//------------------------------------------------------------------------------
// cfgcustomservers.cpp
//------------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "gui/cfgcustomservers.h"
#include "sdeapi/pluginloader.hpp"
#include "main.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QUrl>

const // clear warnings
#include "unknownengine.xpm"

CustomServersConfigBox::CustomServersConfigBox(Config *cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	connect(btnAdd, SIGNAL( clicked() ), this, SLOT( add() ));
	connect(btnRemove, SIGNAL( clicked() ), this, SLOT( remove() ));
	connect(btnSetEngine, SIGNAL( clicked() ), this, SLOT( setEngine() ));

	prepareEnginesComboBox();
}

void CustomServersConfigBox::add()
{
	int pluginIndex = cboEngines->itemData(cboEngines->currentIndex()).toInt();
	const PluginInfo* nfo = (*Main::enginePlugins)[pluginIndex]->info;

	QString engineName = cboEngines->itemText(cboEngines->currentIndex());

	add(engineName, "", nfo->pInterface->defaultServerPort());
}

void CustomServersConfigBox::add(const QString& engineName, const QString& host, unsigned short port)
{
	QList<QStandardItem* > record;

	QStandardItem* engineItem = new QStandardItem();
	setEngineOnItem(engineItem, engineName);

	QString portString = QString::number(port);

	record.append(engineItem);
	record.append(new QStandardItem(host));
	record.append(new QStandardItem(portString));

	model->appendRow(record);
	tvServers->resizeRowsToContents();
}

CustomServersConfigBox::CheckAndFixPorts CustomServersConfigBox::checkAndFixPorts(int firstRow, int lastRow)
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

ConfigurationBoxInfo* CustomServersConfigBox::createStructure(Config *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->confBox = new CustomServersConfigBox(cfg, parent);
	pConfigurationBoxInfo->boxName = tr("Custom Servers");
	pConfigurationBoxInfo->icon = QIcon(":/flags/localhost-small");
	return pConfigurationBoxInfo;
}

void CustomServersConfigBox::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
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

const PluginInfo* CustomServersConfigBox::getPluginInfoForRow(int rowIndex)
{
	QStandardItem* itemEngine = model->item(rowIndex, EngineColumnIndex);
	QString engineName = itemEngine->data().toString();
	int pluginIndex = Main::enginePlugins->pluginIndexFromName(engineName);
	return (*Main::enginePlugins)[pluginIndex]->info;
}

bool CustomServersConfigBox::isPortColumnWithingRange(int leftmostColumnIndex, int rightmostColumnIndex)
{
	return leftmostColumnIndex <= PortColumnIndex && rightmostColumnIndex >= PortColumnIndex;
}

bool CustomServersConfigBox::isPortCorrect(int rowIndex)
{
	const int MIN_PORT = 1;
	const int MAX_PORT = 65535;

	QStandardItem* item = model->item(rowIndex, PortColumnIndex);
	bool ok;
	int port = item->text().toInt(&ok);

	return ok && port >= MIN_PORT && port <= MAX_PORT;
}

void CustomServersConfigBox::prepareEnginesComboBox()
{
	cboEngines->clear();

	for (unsigned i = 0; i < Main::enginePlugins->numPlugins(); ++i)
	{
		const PluginInfo* nfo = (*Main::enginePlugins)[i]->info;
		cboEngines->addItem(nfo->pInterface->icon(), nfo->name, i);
	}

	if (cboEngines->count() > 0)
	{
		cboEngines->setCurrentIndex(0);
	}
}

void CustomServersConfigBox::prepareTable()
{
	model = new QStandardItemModel(this);

	connect(model, SIGNAL( dataChanged(const QModelIndex&, const QModelIndex&) ), this, SLOT( dataChanged(const QModelIndex&, const QModelIndex&) ) );

	QStringList labels;
	labels << "" << "Host" << "Port";
	model->setHorizontalHeaderLabels(labels);

	tvServers->setModel(model);

	tvServers->setColumnWidth(0, 23);
	tvServers->setColumnWidth(1, 180);
	tvServers->setColumnWidth(2, 60);

	tvServers->horizontalHeader()->setHighlightSections(false);
	tvServers->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);

	tvServers->verticalHeader()->hide();
}

void CustomServersConfigBox::readSettings()
{
	prepareTable();

	SettingsData *setting;

	setting = config->setting("CustomServers");
	QList<CustomServerInfo> customServersList;
	
	CustomServers::decodeConfigEntries(setting->string(), customServersList);
	QList<CustomServerInfo>::iterator it;

	for (it = customServersList.begin(); it != customServersList.end(); ++it)
	{
		add(it->engine, it->host, it->port);
	}
}

void CustomServersConfigBox::remove()
{
	QItemSelectionModel* selModel = tvServers->selectionModel();
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

void CustomServersConfigBox::saveSettings()
{
	SettingsData *setting;

	setting = config->setting("CustomServers");
	setting->setValue(tableEntriesEncoded());
}

void CustomServersConfigBox::setEngine()
{
	QItemSelectionModel* sel = tvServers->selectionModel();
	QModelIndexList indexList = sel->selectedRows();

	QModelIndexList::iterator it;
	for (it = indexList.begin(); it != indexList.end(); ++it)
	{
		QStandardItem* item = model->itemFromIndex(*it);
		QString engineName = cboEngines->itemText(cboEngines->currentIndex());
		setEngineOnItem(item, engineName);
	}
}

void CustomServersConfigBox::setEngineOnItem(QStandardItem* item, const QString& engineName)
{
	int engineId = Main::enginePlugins->pluginIndexFromName(engineName);

	item->setData(engineName);
	item->setToolTip(engineName);
	if (engineId >= 0)
	{
		const PluginInfo* nfo = (*Main::enginePlugins)[engineId]->info;
		item->setIcon(nfo->pInterface->icon());
	}
	else
	{
	    item->setIcon(QPixmap(unknownengine_xpm));
	}

	item->setEditable(false);
	item->setText("");
}

void CustomServersConfigBox::setPortToDefault(int rowIndex)
{
	const PluginInfo* pluginInfo = getPluginInfoForRow(rowIndex);
	QString defaultPort = QString::number(pluginInfo->pInterface->defaultServerPort());

	QStandardItem* itemPort = model->item(rowIndex, PortColumnIndex);
	itemPort->setText(defaultPort);
}

QString	CustomServersConfigBox::tableEntriesEncoded()
{
	QStringList allItemsList;
	for (int i = 0; i < model->rowCount(); ++i)
	{
		QStringList itemList;
		QStandardItem* item = model->item(i, EngineColumnIndex);

		itemList << QUrl::toPercentEncoding(item->data().toString(), "", "()");

		item = model->item(i, AddressColumnIndex);
		itemList << QUrl::toPercentEncoding(item->text(), "", "()");

		item = model->item(i, PortColumnIndex);
		itemList << item->text();

		allItemsList << QString("(" + itemList.join(";") + ")");
	}

	return allItemsList.join(";");
}
