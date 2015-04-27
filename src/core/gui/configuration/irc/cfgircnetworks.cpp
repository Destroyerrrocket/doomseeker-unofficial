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
#include "ui_cfgircnetworks.h"
#include "gui/configuration/irc/cfgircdefinenetworkdialog.h"
#include "irc/configuration/chatnetworkscfg.h"
#include "irc/configuration/ircconfig.h"
#include "irc/entities/ircnetworkentity.h"
#include "irc/chatlogs.h"
#include "qtmetapointer.h"
#include <QItemDelegate>
#include <QStandardItemModel>

DClass<CFGIRCNetworks> : public Ui::CFGIRCNetworks
{
};

DPointered(CFGIRCNetworks)

CFGIRCNetworks::CFGIRCNetworks(QWidget* parent)
: ConfigurationBaseBox(parent)
{
	d->setupUi(this);

	connect(d->btnAdd, SIGNAL( clicked() ), this, SLOT( addButtonClicked() ) );
	connect(d->btnEdit, SIGNAL( clicked() ), this, SLOT( editButtonClicked() ) );
	connect(d->btnRemove, SIGNAL( clicked() ), this, SLOT( removeButtonClicked() ) );
	connect(d->gridNetworks, SIGNAL( doubleClicked(const QModelIndex&) ), SLOT( tableDoubleClicked(const QModelIndex&) ) );

	// Crash prevention measure.
	d->gridNetworks->setModel(new QStandardItemModel(this));
}

CFGIRCNetworks::~CFGIRCNetworks()
{
	cleanUpTable();
}

void CFGIRCNetworks::addButtonClicked()
{
	CFGIRCDefineNetworkDialog dialog(this);
	dialog.setExistingNetworks(networksAsQList());
	if (dialog.exec() == QDialog::Accepted)
	{
		IRCNetworkEntity* pNetworkEntity = new IRCNetworkEntity();
		*pNetworkEntity = dialog.getNetworkEntity();

		addRecord(pNetworkEntity);
		saveNetworks();
	}
}

void CFGIRCNetworks::addRecord(IRCNetworkEntity* pNetworkEntity)
{
	QStandardItemModel* pModel = (QStandardItemModel*)d->gridNetworks->model();
	pModel->appendRow(generateTableRecord(pNetworkEntity));

	this->d->gridNetworks->resizeRowsToContents();

	if (this->d->gridNetworks->model()->rowCount() == 1)
	{
		this->d->gridNetworks->resizeColumnToContents(0);
	}
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
		dialog.setExistingNetworks(networksAsQList());
		if (dialog.exec() == QDialog::Accepted)
		{
			IRCNetworkEntity editedNetwork = dialog.getNetworkEntity();
			if (ChatLogs().renameNetwork(this, pNetwork->description(), editedNetwork.description()))
			{
				*pNetwork = dialog.getNetworkEntity();
				this->updateRecord(this->selectedRow());
				saveNetworks();
			}
		}
	}
}

QList<QStandardItem*> CFGIRCNetworks::generateTableRecord(IRCNetworkEntity* pNetworkEntity)
{
	QList<QStandardItem*> itemArray;
	QStandardItem* pItem;

	// Boolean item.
	pItem = new QStandardItem();
	pItem->setCheckable(true);
	pItem->setCheckState(pNetworkEntity->isAutojoinNetwork() ? Qt::Checked : Qt::Unchecked);
	pItem->setToolTip("Autojoin?");
	itemArray << pItem;

	void* pointer = pNetworkEntity;
	QtMetaPointer metaPointer = pointer;
	QVariant variantPointer = qVariantFromValue(metaPointer);
	pItem->setData(variantPointer);

	pItem = new QStandardItem(pNetworkEntity->description());
	itemArray << pItem;

	pItem = new QStandardItem(QString("%1:%2").arg(pNetworkEntity->address())
		.arg(pNetworkEntity->port()));

	itemArray << pItem;

	return itemArray;
}

IRCNetworkEntity* CFGIRCNetworks::network(int row) const
{
	QStandardItemModel* pModel = (QStandardItemModel*)d->gridNetworks->model();
	IRCNetworkEntity* pNetwork = obtainNetworkEntity(pModel->item(row));
	pNetwork->setAutojoinNetwork((pModel->item(row, 0)->checkState() == Qt::Checked));

	return pNetwork;
}

QVector<IRCNetworkEntity*> CFGIRCNetworks::networks()
{
	QVector<IRCNetworkEntity*> entityArray;

	QStandardItemModel* pModel = (QStandardItemModel*)d->gridNetworks->model();
	for (int i = 0; i < pModel->rowCount(); ++i)
	{
		IRCNetworkEntity* pEntity = this->network(i);

		entityArray << pEntity;
	}

	return entityArray;
}

QList<IRCNetworkEntity> CFGIRCNetworks::networksAsQList() const
{
	QList<IRCNetworkEntity> result;
	for (int row = 0; row < d->gridNetworks->model()->rowCount(); ++row)
	{
		result << *network(row);
	}
	return result;
}

IRCNetworkEntity* CFGIRCNetworks::obtainNetworkEntity(QStandardItem* pItem) const
{
	QtMetaPointer metaPointer = pItem->data().value<QtMetaPointer>();
	void* pointer = metaPointer;
	IRCNetworkEntity* pEntity = (IRCNetworkEntity*)pointer;

	return pEntity;
}

void CFGIRCNetworks::prepareTable()
{
	cleanUpTable();

	QStandardItemModel* pModel = new QStandardItemModel(this);

	QStringList labels;
	labels << "" << tr("Description") << tr("Address");
	pModel->setHorizontalHeaderLabels(labels);

	d->gridNetworks->setModel(pModel);

	d->gridNetworks->setColumnWidth(1, 180);
	d->gridNetworks->setColumnWidth(2, 180);

	d->gridNetworks->horizontalHeader()->setHighlightSections(false);
#if QT_VERSION >= 0x050000
	d->gridNetworks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
#else
	d->gridNetworks->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
#endif

	d->gridNetworks->verticalHeader()->hide();
}

void CFGIRCNetworks::readSettings()
{
	prepareTable();

	QList<IRCNetworkEntity> cfgNetworks = ChatNetworksCfg().networks();
	for (int i = 0; i < cfgNetworks.size(); ++i)
	{
		// Remember that pointers are stored within the table.
		IRCNetworkEntity* pCopy = new IRCNetworkEntity();
		*pCopy = cfgNetworks[i];

		addRecord(pCopy);
	}

	d->leQuitMessage->setText(gIRCConfig.personal.quitMessage);
}

void CFGIRCNetworks::removeButtonClicked()
{
	int row = this->selectedRow();

	if (row >= 0)
	{
		QStandardItemModel* pModel = (QStandardItemModel*)d->gridNetworks->model();
		pModel->removeRow(row);
	}
}

void CFGIRCNetworks::saveNetworks()
{
	ChatNetworksCfg().setNetworks(networksAsQList());
}

void CFGIRCNetworks::saveSettings()
{
	saveNetworks();

	gIRCConfig.personal.quitMessage = d->leQuitMessage->text().trimmed();
}

IRCNetworkEntity* CFGIRCNetworks::selectedNetwork()
{
	QStandardItemModel* pModel = (QStandardItemModel*)d->gridNetworks->model();
	QItemSelectionModel* pSelectionModel = d->gridNetworks->selectionModel();
	QModelIndexList indexList = pSelectionModel->selectedRows();

	if (!indexList.empty())
	{
		QModelIndex index = indexList[0];
		return network(index.row());
	}

	return NULL;
}

int CFGIRCNetworks::selectedRow()
{
	QStandardItemModel* pModel = (QStandardItemModel*)d->gridNetworks->model();
	QItemSelectionModel* pSelectionModel = d->gridNetworks->selectionModel();
	QModelIndexList indexList = pSelectionModel->selectedRows();

	if (!indexList.empty())
	{
		return indexList[0].row();
	}

	return -1;
}

void CFGIRCNetworks::tableDoubleClicked(const QModelIndex& index)
{
	editButtonClicked();
}

void CFGIRCNetworks::updateRecord(int row)
{
	QStandardItemModel* pModel = (QStandardItemModel*)d->gridNetworks->model();
	QStandardItem* pItemDescription = pModel->item(row, 1);

	IRCNetworkEntity* pNetwork = this->network(row);

	pItemDescription->setText(pNetwork->description());

	QStandardItem* pItemAddress = pModel->item(row, 2);
	pItemAddress->setText(QString("%1:%2").arg(pNetwork->address()).arg(pNetwork->port()));

	this->d->gridNetworks->resizeRowsToContents();
}
