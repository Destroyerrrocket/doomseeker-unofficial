//------------------------------------------------------------------------------
// cfgircnetworks.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "cfgircnetworks.h"
#include "gui/configuration/irc/cfgircdefinenetworkdialog.h"
#include "irc/configuration/ircconfig.h"
#include "qtmetapointer.h"
#include <QStandardItemModel>

CFGIRCNetworks::CFGIRCNetworks(QWidget* parent)
: ConfigurationBaseBox(parent)
{
	setupUi(this);
	
	connect(btnAdd, SIGNAL( clicked() ), this, SLOT( addButtonClicked() ) );
	connect(btnEdit, SIGNAL( clicked() ), this, SLOT( editButtonClicked() ) );
	connect(btnRemove, SIGNAL( clicked() ), this, SLOT( removeButtonClicked() ) );

	// Crash prevention measure.
	gridNetworks->setModel(new QStandardItemModel(this));
}

CFGIRCNetworks::~CFGIRCNetworks()
{
	QVector<IRCNetworkEntity*> networksArray = networks();
	foreach (IRCNetworkEntity* pEntity, networksArray)
	{
		delete pEntity;
	}
}

void CFGIRCNetworks::addButtonClicked()
{
	CFGIRCDefineNetworkDialog dialog(this);
	if (dialog.exec() == QDialog::Accepted)
	{
		IRCNetworkEntity* pNetworkEntity = new IRCNetworkEntity();
		*pNetworkEntity = dialog.getNetworkEntity();
		
		addRecord(pNetworkEntity);
	}
	
}

void CFGIRCNetworks::addRecord(IRCNetworkEntity* pNetworkEntity)
{
	QStandardItemModel* pModel = (QStandardItemModel*)gridNetworks->model();
	pModel->appendRow(generateTableRecord(pNetworkEntity));
	
	this->gridNetworks->resizeRowsToContents();
}

void CFGIRCNetworks::cleanUpTable()
{
	QVector<IRCNetworkEntity*> networksArray = networks();
	foreach (IRCNetworkEntity* pEntity, networksArray)
	{
		delete pEntity;
	}
}

void CFGIRCNetworks::editButtonClicked()
{
	IRCNetworkEntity* pNetwork = this->selectedNetwork();
	if (pNetwork != NULL)
	{
		CFGIRCDefineNetworkDialog dialog(*pNetwork, this);
		if (dialog.exec() == QDialog::Accepted)
		{
			*pNetwork = dialog.getNetworkEntity();
			this->updateRecord(this->selectedRow());
		}
	}
}

QList<QStandardItem*> CFGIRCNetworks::generateTableRecord(IRCNetworkEntity* pNetworkEntity)
{
	QList<QStandardItem*> itemArray;
	QStandardItem* pItem;
	
	pItem = new QStandardItem(pNetworkEntity->description);
	
	void* pointer = pNetworkEntity;
	QtMetaPointer metaPointer = pointer;
	QVariant variantPointer = qVariantFromValue(metaPointer);
	pItem->setData(variantPointer);
	
	itemArray << pItem;
	
	pItem = new QStandardItem(QString("%1:%2").arg(pNetworkEntity->address)
		.arg(pNetworkEntity->port));
		
	itemArray << pItem;
	
	return itemArray;
}

IRCNetworkEntity* CFGIRCNetworks::network(int row)
{
	QStandardItemModel* pModel = (QStandardItemModel*)gridNetworks->model();
	return obtainNetworkEntity(pModel->item(row));
}

QVector<IRCNetworkEntity*> CFGIRCNetworks::networks()
{
	QVector<IRCNetworkEntity*> entityArray;

	QStandardItemModel* pModel = (QStandardItemModel*)gridNetworks->model();
	for (int i = 0; i < pModel->rowCount(); ++i)
	{
		// The first column will always store the pointer to the networkEntity
		// object.
		QStandardItem* pItem = pModel->item(i);
		IRCNetworkEntity* pEntity = obtainNetworkEntity(pItem);
	
		entityArray << pEntity;
	}
	
	return entityArray;
}

IRCNetworkEntity* CFGIRCNetworks::obtainNetworkEntity(QStandardItem* pItem)
{
	QtMetaPointer metaPointer = qVariantValue<QtMetaPointer>(pItem->data());
	void* pointer = metaPointer;
	IRCNetworkEntity* pEntity = (IRCNetworkEntity*)pointer;
	
	return pEntity;
}

void CFGIRCNetworks::prepareTable()
{
	cleanUpTable();
	
	QStandardItemModel* pModel = new QStandardItemModel(this);

	QStringList labels;
	labels << tr("Description") << tr("Address");
	pModel->setHorizontalHeaderLabels(labels);

	gridNetworks->setModel(pModel);

	gridNetworks->setColumnWidth(0, 180);
	gridNetworks->setColumnWidth(1, 180);

	gridNetworks->horizontalHeader()->setHighlightSections(false);
	gridNetworks->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);

	gridNetworks->verticalHeader()->hide();
}
		
void CFGIRCNetworks::readSettings()
{
	prepareTable();
	
	const QVector<IRCNetworkEntity>& cfgNetworks = gIRCConfig.networks.networks;
	for (int i = 0; i < cfgNetworks.size(); ++i)
	{
		// Remember that pointers are stored within the table.
		IRCNetworkEntity* pCopy = new IRCNetworkEntity();
		*pCopy = cfgNetworks[i];
		
		addRecord(pCopy);
	}
}

void CFGIRCNetworks::removeButtonClicked()
{
	int row = this->selectedRow();

	if (row >= 0)
	{
		QStandardItemModel* pModel = (QStandardItemModel*)gridNetworks->model();
		pModel->removeRow(row);
	}
}
		
void CFGIRCNetworks::saveSettings()
{
	QVector<IRCNetworkEntity*> networksArray = networks();
	gIRCConfig.networks.networks.clear();
	
	foreach (IRCNetworkEntity* pEntity, networksArray)
	{
		gIRCConfig.networks.networks << *pEntity;
	}
}

IRCNetworkEntity* CFGIRCNetworks::selectedNetwork()
{
	QStandardItemModel* pModel = (QStandardItemModel*)gridNetworks->model();
	QItemSelectionModel* pSelectionModel = gridNetworks->selectionModel();
	QModelIndexList indexList = pSelectionModel->selectedRows();

	if (!indexList.empty())
	{
		QModelIndex index = indexList[0];
		QStandardItem* pItem = pModel->item(index.row());
		
		return obtainNetworkEntity(pItem);
	}
	
	return NULL;
}
	
int CFGIRCNetworks::selectedRow()
{
	QStandardItemModel* pModel = (QStandardItemModel*)gridNetworks->model();
	QItemSelectionModel* pSelectionModel = gridNetworks->selectionModel();
	QModelIndexList indexList = pSelectionModel->selectedRows();

	if (!indexList.empty())
	{
		return indexList[0].row();
	}
	
	return -1;
}
	
void CFGIRCNetworks::updateRecord(int row)
{
	QStandardItemModel* pModel = (QStandardItemModel*)gridNetworks->model();
	QStandardItem* pItemDescription = pModel->item(row, 0);
	
	IRCNetworkEntity* pNetwork = this->network(row);
	
	pItemDescription->setText(pNetwork->description);
	
	QStandardItem* pItemAddress = pModel->item(row, 1);
	pItemAddress->setText(QString("%1:%2").arg(pNetwork->address).arg(pNetwork->port));
	
	this->gridNetworks->resizeRowsToContents();
}
