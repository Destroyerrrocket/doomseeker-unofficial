//------------------------------------------------------------------------------
// configurationdialog.cpp
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
#include "configurationdialog.h"
#include "ui_configurationdialog.h"

#include "configurationbasebox.h"
#include "qtmetapointer.h"
#include <Qt>
#include <QDebug>
#include <QKeyEvent>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QAbstractButton>

class ConfigurationDialog::PrivData : public Ui::ConfigurationDialog
{
public:
	QList<ConfigurationBaseBox*> configBoxesList;
	QWidget* currentlyDisplayedCfgBox;
};

ConfigurationDialog::ConfigurationDialog(QWidget* parent)
: QDialog(parent)
{
	d = new PrivData;
	d->setupUi(this);

	d->tvOptionsList->setHeaderHidden(true);
	d->tvOptionsList->setModel(new QStandardItemModel(this));

	d->currentlyDisplayedCfgBox = NULL;
	connect(d->buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT ( btnClicked(QAbstractButton *) ));
	this->connect(d->tvOptionsList->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
		SLOT(onOptionListCurrentChanged(QModelIndex, QModelIndex)));
}

ConfigurationDialog::~ConfigurationDialog()
{
	for(int i = 0; i < d->configBoxesList.count(); ++i)
	{
		delete d->configBoxesList[i];
	}

	delete d;
}

QStandardItem* ConfigurationDialog::addConfigurationBox(QStandardItem* rootItem, ConfigurationBaseBox* pConfigurationBox, int position)
{
	if (!canConfigurationBoxBeAddedToList(pConfigurationBox))
	{
		return NULL;
	}

	QStandardItemModel* pModel = (QStandardItemModel*)d->tvOptionsList->model();
	if (rootItem == NULL)
	{
		rootItem = pModel->invisibleRootItem();
	}

	if (!this->hasItemOnList(rootItem))
	{
		return NULL;
	}

	QStandardItem* pNewItem = new QStandardItem(pConfigurationBox->name());
	pNewItem->setIcon(pConfigurationBox->icon());

	void* pointerConfigurationBox = pConfigurationBox;
	QtMetaPointer metaPointer = pointerConfigurationBox;
	QVariant variantConfigurationBox = qVariantFromValue(metaPointer);
	pNewItem->setData(variantConfigurationBox);

	if (position < 0)
	{
		rootItem->appendRow(pNewItem);
	}
	else
	{
		rootItem->insertRow(position, pNewItem);
	}

	d->configBoxesList.push_back(pConfigurationBox);

	return pNewItem;
}

QStandardItem* ConfigurationDialog::addLabel(QStandardItem* rootItem, const QString& label, int position)
{
	QStandardItemModel* pModel = (QStandardItemModel*)d->tvOptionsList->model();
	if (rootItem == NULL)
	{
		rootItem = pModel->invisibleRootItem();
	}

	if (!this->hasItemOnList(rootItem))
	{
		return NULL;
	}

	QtMetaPointer metaPointer = (void*)NULL;
	QVariant variantMetaPointer = qVariantFromValue(metaPointer);

	QStandardItem* pNewItem = new QStandardItem(label);
	pNewItem->setData(variantMetaPointer);

	if (position < 0)
	{
		rootItem->appendRow(pNewItem);
	}
	else
	{
		rootItem->insertRow(position, pNewItem);
	}

	return pNewItem;
}

void ConfigurationDialog::btnClicked(QAbstractButton *button)
{
	// Figure out what button we pressed and perform its action.
	switch(d->buttonBox->standardButton(button))
	{
		default:
			break;

		case QDialogButtonBox::Ok: // Also does the same as Apply
			if (this->validate())
			{
				this->accept();
				this->saveSettings();
			}
			break;

		case QDialogButtonBox::Apply:
			if (this->validate())
			{
				this->saveSettings();
			}
			break;

		case QDialogButtonBox::Cancel:
			this->reject();
			break;
	}
}

bool ConfigurationDialog::canConfigurationBoxBeAddedToList(ConfigurationBaseBox* pConfigurationBox)
{
	return isConfigurationBoxInfoValid(pConfigurationBox) && !isConfigurationBoxOnTheList(pConfigurationBox);
}

bool ConfigurationDialog::isConfigurationBoxInfoValid(ConfigurationBaseBox* pConfigurationBox)
{
	return pConfigurationBox != NULL && !pConfigurationBox->name().isEmpty();
}

bool ConfigurationDialog::isConfigurationBoxOnTheList(ConfigurationBaseBox* pConfigurationBox)
{
	foreach (ConfigurationBaseBox* pBoxOnTheList, d->configBoxesList)
	{
		if (pConfigurationBox == pBoxOnTheList)
		{
			return true;
		}
	}

	return false;
}

void ConfigurationDialog::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			// Suppress the dialog being accepted on pressing ENTER key.
			// Dialog would close even in line edits that had "returnPressed()"
			// signals connected. That wasn't good.
			e->ignore();
			break;
		default:
			QDialog::keyPressEvent(e);
	}
}

bool ConfigurationDialog::hasItemOnList(QStandardItem* pItem) const
{
	if (pItem == NULL)
	{
		return false;
	}

	QStandardItemModel* pModel = (QStandardItemModel*)d->tvOptionsList->model();

	// Calling index methods on the invisible root items will always return
	// invalid values.

	return pModel->invisibleRootItem() == pItem
		|| pModel->indexFromItem(pItem).isValid();
}

void ConfigurationDialog::onOptionListCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	if (current.isValid() && current != previous)
	{
		switchToItem(current);
	}
}

void ConfigurationDialog::switchToItem(const QModelIndex& index)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(d->tvOptionsList->model());
	QStandardItem* item = model->itemFromIndex(index);

	QtMetaPointer metaPointer = qVariantValue<QtMetaPointer>(item->data());
	void* pointer = metaPointer;
	ConfigurationBaseBox* pConfigBox = (ConfigurationBaseBox*)pointer;

	// Something with sense was selected, display this something
	// and hide previous box.
	if (isConfigurationBoxInfoValid(pConfigBox))
	{
		if (!pConfigBox->areSettingsAlreadyRead())
		{
			pConfigBox->read();
		}
		pConfigBox->setAllowSave(true);
		showConfigurationBox(pConfigBox);
	}
}

QTreeView* ConfigurationDialog::optionsTree()
{
	return d->tvOptionsList;
}

void ConfigurationDialog::saveSettings()
{
	// Iterate through every engine and execute it's saving method
	for (int i = 0; i < d->configBoxesList.count(); ++i)
	{
		d->configBoxesList[i]->save();
	}

	doSaveSettings();

	if(isVisible())
	{
		// Allow panels such as the one for Wadseeker update their contents.
		for (int i = 0; i < d->configBoxesList.count(); ++i)
		{
			d->configBoxesList[i]->read();
		}
	}
}


void ConfigurationDialog::showConfigurationBox(ConfigurationBaseBox* widget)
{
	if (d->currentlyDisplayedCfgBox != NULL)
	{
		d->currentlyDisplayedCfgBox->hide();
		d->mainPanel->setTitle(QString());
	}
	d->currentlyDisplayedCfgBox = widget;

	if (widget != NULL)
	{
		d->mainPanel->layout()->addWidget(widget);
		d->mainPanel->setTitle(widget->title());
		widget->show();
	}
}
