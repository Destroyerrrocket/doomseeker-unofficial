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
#include "qtmetapointer.h"
#include <Qt>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QAbstractButton>

ConfigurationDialog::ConfigurationDialog(QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	tvOptionsList->setHeaderHidden(true);
	tvOptionsList->setModel(new QStandardItemModel(this));

	currentlyDisplayedCfgBox = NULL;
	connect(tvOptionsList, SIGNAL( clicked(const QModelIndex&) ), this, SLOT( optionListClicked(const QModelIndex&) ) );
	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT ( btnClicked(QAbstractButton *) ));
}

ConfigurationDialog::~ConfigurationDialog()
{
	for(int i = 0; i < configBoxesList.count(); ++i)
	{
		delete configBoxesList[i];
	}
}

QStandardItem* ConfigurationDialog::addConfigurationBox(QStandardItem* rootItem, ConfigurationBaseBox* pConfigurationBox, int position)
{
	if (!canConfigurationBoxBeAddedToList(pConfigurationBox))
	{
		return NULL;
	}
	
	QStandardItemModel* pModel = (QStandardItemModel*)tvOptionsList->model();	
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
	QVariant variantConfigurationBox = qVariantFromValue(pointerConfigurationBox);
	pNewItem->setData(variantConfigurationBox);
	
	if (position < 0)
	{
		rootItem->appendRow(pNewItem);
	}
	else
	{
		rootItem->insertRow(position, pNewItem);
	}

	configBoxesList.push_back(pConfigurationBox);
	connect(pConfigurationBox, SIGNAL( wantChangeDefaultButton(QPushButton*) ), 
		SLOT( wantChangeDefaultButton(QPushButton*) ) );

	return pNewItem;
}

QStandardItem* ConfigurationDialog::addLabel(QStandardItem* rootItem, const QString& label, int position)
{
	QStandardItemModel* pModel = (QStandardItemModel*)tvOptionsList->model();
	if (rootItem == NULL)
	{
		rootItem = pModel->invisibleRootItem();
	}
	
	if (!this->hasItemOnList(rootItem))
	{
		return NULL;
	}
	
	QStandardItem* pNewItem = new QStandardItem(label);
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
	foreach (ConfigurationBaseBox* pBoxOnTheList, configBoxesList)
	{
		if (pConfigurationBox == pBoxOnTheList)
		{
			return true;
		}
	}
	
	return false;
}

bool ConfigurationDialog::hasItemOnList(QStandardItem* pItem) const
{
	if (pItem == NULL)
	{
		return NULL;
	}
	
	QStandardItemModel* pModel = (QStandardItemModel*)tvOptionsList->model();
	
	// Calling index methods on the invisible root items will always return
	// invalid values.
	
	return pModel->invisibleRootItem() == pItem 
		|| pModel->indexFromItem(pItem).isValid();
}

void ConfigurationDialog::optionListClicked(const QModelIndex& index)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(tvOptionsList->model());
	QStandardItem* item = model->itemFromIndex(index);

	void* pointer = qVariantValue<QtMetaPointer>(item->data());
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

void ConfigurationDialog::saveSettings()
{
	// Iterate through every engine and execute it's saving method
	for (int i = 0; i < configBoxesList.count(); ++i)
	{
		configBoxesList[i]->save();
	}
	
	doSaveSettings();

	if(isVisible())
	{
		// Allow panels such as the one for Wadseeker update their contents.
		for (int i = 0; i < configBoxesList.count(); ++i)
		{
			configBoxesList[i]->read();
		}
	}
}


void ConfigurationDialog::showConfigurationBox(QWidget* widget)
{
	if (currentlyDisplayedCfgBox != NULL)
	{
		currentlyDisplayedCfgBox->hide();
	}
	currentlyDisplayedCfgBox = widget;

	if (widget != NULL)
	{
		mainPanel->layout()->addWidget(widget);
		widget->show();
	}
}

void ConfigurationDialog::wantChangeDefaultButton(QPushButton* button)
{
	if (button == NULL)
	{
		buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
	}
	else
	{
		button->setDefault(true);
	}
}
