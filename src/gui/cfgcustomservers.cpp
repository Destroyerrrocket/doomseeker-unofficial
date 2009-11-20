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

	add(engineName, "", nfo->pInterface->generalEngineInfo().defaultServerPort);
}

void CustomServersConfigBox::add(const QString& engineName, const QString& host, short port)
{
	QList<QStandardItem* > record;

	QStandardItem* engine = new QStandardItem();
	setEngineOnItem(engine, engineName);

	record.append(engine);
	record.append(new QStandardItem(host));
	record.append(new QStandardItem( QString::number(port) ));

	model->appendRow(record);
	tvServers->resizeRowsToContents();
}

ConfigurationBoxInfo* CustomServersConfigBox::createStructure(Config *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->confBox = new CustomServersConfigBox(cfg, parent);
	ec->boxName = tr("Custom Servers");
	return ec;
}

void CustomServersConfigBox::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
	if (bottomRight.column() < 2)
		return;
	else
	{
		// Check if port is in correct range
		for (int i = topLeft.row(); i <= bottomRight.row(); ++i)
		{
			QStandardItem* item = model->item(i, 2);
			bool ok;
			int port = item->text().toInt(&ok);
			if (!ok || port < 1 || port > 65535)
			{
				QMessageBox::warning(this, tr("Doomseeker - custom servers"), tr("Port must be from range 1 - 65535"));

				// Set port to default:
				QStandardItem* itemEng = model->item(i, 0);
				int pluginIndex = itemEng->data().toInt();
				const PluginInfo* nfo = (*Main::enginePlugins)[pluginIndex]->info;
				item->setText(QString::number(nfo->pInterface->generalEngineInfo().defaultServerPort));
			}
		}
	}
}

void CustomServersConfigBox::prepareEnginesComboBox()
{
	cboEngines->clear();

	for (int i = 0; i < Main::enginePlugins->numPlugins(); ++i)
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
	QList<CustomServerInfo>* entrylist = CustomServers::decodeConfigEntries(setting->string());
	QList<CustomServerInfo>::iterator it;

	for (it = entrylist->begin(); it != entrylist->end(); ++it)
	{
		add(it->engine, it->host, it->port);
	}

	delete entrylist;
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

QString	CustomServersConfigBox::tableEntriesEncoded()
{
	QStringList allItemsList;
	for (int i = 0; i < model->rowCount(); ++i)
	{
		QStringList itemList;
		QStandardItem* item = model->item(i, 0);

		itemList << QUrl::toPercentEncoding(item->data().toString(), "", "()");

		item = model->item(i, 1);
		itemList << QUrl::toPercentEncoding(item->text(), "", "()");

		item = model->item(i, 2);
		itemList << item->text();

		allItemsList << QString("(" + itemList.join(";") + ")");
	}

	return allItemsList.join(";");
}
