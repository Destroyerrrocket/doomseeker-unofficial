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
#include <cassert>
#include <Qt>
#include <QDebug>
#include <QKeyEvent>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QAbstractButton>

DClass<ConfigurationDialog> : public Ui::ConfigurationDialog
{
public:
	enum Column
	{
		COL_FIRST = 0,
		COL_META = 0,
		COL_NAME = 0,
		COL_VALIDATION = 1,
		COLSIZE,
	};

	QList<ConfigurationBaseBox*> configBoxesList;
	ConfigurationBaseBox* currentlyDisplayedCfgBox;

	QModelIndex findPageModelIndex(const QModelIndex &rootIndex, ConfigurationBaseBox *page)
	{
		QStandardItemModel *model = static_cast<QStandardItemModel*>(tvOptionsList->model());
		for (int row = 0; row < model->rowCount(rootIndex); ++row)
		{
			QModelIndex index = model->index(row, COL_META, rootIndex);
			ConfigurationBaseBox *pageAtIndex = pageFromIndex(index);
			if (pageAtIndex == page)
			{
				return index;
			}
			QModelIndex childIndex = findPageModelIndex(index, page);
			if (childIndex.isValid())
			{
				return childIndex;
			}
		}
		return QModelIndex();
	}

	ConfigurationBaseBox *pageFromIndex(const QModelIndex &index)
	{
		QModelIndex pageIndex = index.sibling(index.row(), COL_META);
		QtMetaPointer metaPointer = pageIndex.data(Qt::UserRole).value<QtMetaPointer>();
		void* pointer = metaPointer;
		return static_cast<ConfigurationBaseBox*>(pointer);
	}

	QIcon validationIcon(ConfigurationBaseBox::Validation validation) const
	{
		switch (validation)
		{
		case ConfigurationBaseBox::VALIDATION_ERROR:
			return QIcon(":icons/exclamation_16.png");
		default:
			return QIcon();
		}
	}
};

DPointered(ConfigurationDialog)

ConfigurationDialog::ConfigurationDialog(QWidget* parent)
: QDialog(parent)
{
	d->setupUi(this);

	QStandardItemModel *model = new QStandardItemModel(this);
	for (int column = PrivData<ConfigurationDialog>::COL_FIRST;
		column < PrivData<ConfigurationDialog>::COLSIZE; ++column)
	{
		model->setHorizontalHeaderItem(column, new QStandardItem());
	}
	d->tvOptionsList->setModel(model);

#if QT_VERSION >= 0x050000
	d->tvOptionsList->header()->setSectionResizeMode(
		PrivData<ConfigurationDialog>::COL_NAME, QHeaderView::Stretch);
	d->tvOptionsList->header()->setSectionResizeMode(
		PrivData<ConfigurationDialog>::COL_VALIDATION, QHeaderView::ResizeToContents);
#else
	d->tvOptionsList->header()->setResizeMode(
		PrivData<ConfigurationDialog>::COL_NAME, QHeaderView::Stretch);
	d->tvOptionsList->header()->setResizeMode(
		PrivData<ConfigurationDialog>::COL_VALIDATION, QHeaderView::ResizeToContents);
#endif

	d->currentlyDisplayedCfgBox = NULL;
	connect(d->buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT ( btnClicked(QAbstractButton *) ));
	this->connect(d->tvOptionsList->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
		SLOT(switchToItem(QModelIndex, QModelIndex)));
}

ConfigurationDialog::~ConfigurationDialog()
{
	for(int i = 0; i < d->configBoxesList.count(); ++i)
	{
		delete d->configBoxesList[i];
	}
}

QStandardItem* ConfigurationDialog::addConfigurationBox(
	QStandardItem* rootItem, ConfigurationBaseBox* pConfigurationBox, int position)
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

	QList<QStandardItem*> row;

	QStandardItem* nameItem = new QStandardItem(pConfigurationBox->name());
	nameItem->setIcon(pConfigurationBox->icon());

	row.insert(PrivData<ConfigurationDialog>::COL_NAME, nameItem);

	QtMetaPointer metaPointer = pConfigurationBox;
	QVariant variantConfigurationBox = qVariantFromValue(metaPointer);
	nameItem->setData(variantConfigurationBox, Qt::UserRole);

	row.insert(PrivData<ConfigurationDialog>::COL_VALIDATION, new QStandardItem());

	if (position < 0)
	{
		rootItem->appendRow(row);
	}
	else
	{
		rootItem->insertRow(position, row);
	}
	
	d->configBoxesList << pConfigurationBox;
	this->connect(pConfigurationBox, SIGNAL(validationRequested()),
		SLOT(onPageValidationRequested()));

	if (!pConfigurationBox->areSettingsAlreadyRead())
	{
		pConfigurationBox->read();
	}
	validatePage(pConfigurationBox);

	return nameItem;
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

	QList<QStandardItem*> row;

	QStandardItem* nameItem = new QStandardItem(label);
	nameItem->setData(variantMetaPointer, Qt::UserRole);
	row.insert(PrivData<ConfigurationDialog>::COL_NAME, nameItem);
	row.insert(PrivData<ConfigurationDialog>::COL_VALIDATION, new QStandardItem());

	if (position < 0)
	{
		rootItem->appendRow(row);
	}
	else
	{
		rootItem->insertRow(position, row);
	}

	return nameItem;
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

QModelIndex ConfigurationDialog::findPageModelIndex(ConfigurationBaseBox *page)
{
	QStandardItemModel *model = static_cast<QStandardItemModel*>(d->tvOptionsList->model());
	return d->findPageModelIndex(model->indexFromItem(model->invisibleRootItem()), page);
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

void ConfigurationDialog::onPageValidationRequested()
{
	ConfigurationBaseBox *page = qobject_cast<ConfigurationBaseBox*>(sender());
	assert(page != NULL);
	validatePage(page);
}

void ConfigurationDialog::switchToItem(const QModelIndex& current, const QModelIndex &previous)
{
	if (current.isValid() && current != previous)
	{
		ConfigurationBaseBox* pConfigBox = d->pageFromIndex(current);

		// Something with sense was selected, display this something
		// and hide previous box.
		if (isConfigurationBoxInfoValid(pConfigBox))
		{
			showConfigurationBox(pConfigBox);
		}
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
			validatePage(d->configBoxesList[i]);
		}
	}
}

void ConfigurationDialog::showConfigurationBox(ConfigurationBaseBox* widget)
{
	if (d->currentlyDisplayedCfgBox != NULL)
	{
		validatePage(d->currentlyDisplayedCfgBox);
		d->currentlyDisplayedCfgBox->hide();
		d->mainPanel->setTitle(QString());
	}
	d->currentlyDisplayedCfgBox = widget;

	if (widget != NULL)
	{
		widget->setAllowSave(true);
		validatePage(widget);
		d->mainPanel->layout()->addWidget(widget);
		d->mainPanel->setTitle(widget->title());
		widget->show();
	}
}

void ConfigurationDialog::validatePage(ConfigurationBaseBox *page)
{
	assert(page != NULL);
	QModelIndex pageIndex = findPageModelIndex(page);
	assert(pageIndex.isValid());

	QModelIndex validationIndex = pageIndex.sibling(pageIndex.row(), PrivData<ConfigurationDialog>::COL_VALIDATION);
	assert(validationIndex.isValid());

	QStandardItemModel *model = static_cast<QStandardItemModel*>(d->tvOptionsList->model());
	QStandardItem *validationItem = model->itemFromIndex(validationIndex);
	validationItem->setIcon(d->validationIcon(page->validate()));
}
