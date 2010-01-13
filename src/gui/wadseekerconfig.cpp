//------------------------------------------------------------------------------
// wadseekerconfig.cpp
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

#include "gui/wadseekerconfig.h"
#include "wadseeker/wadseeker.h"
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

WadseekerConfigBox::WadseekerConfigBox(Config* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	lstUrls->setModel(new QStandardItemModel());
	cbTargetDirectory->setCompleter(new QCompleter(new QDirModel()));

	connect(btnIdgamesURLDefault, SIGNAL( clicked() ), this, SLOT( btnIdgamesURLDefaultClicked() ) );
	connect(btnUrlAdd, SIGNAL( clicked() ), this, SLOT( btnUrlAddClicked() ) );
	connect(btnUrlDefault, SIGNAL( clicked() ), this, SLOT( btnUrlDefaultClicked() ) );
	connect(btnUrlRemove, SIGNAL( clicked() ), this, SLOT( btnUrlRemoveClicked() ) );
	connect(QApplication::instance(), SIGNAL( focusChanged(QWidget*, QWidget*) ), this, SLOT( focusChanged(QWidget*, QWidget*) ));

	cboIdgamesPriority->addItem("After all sites");
	cboIdgamesPriority->addItem("After custom site");
}

void WadseekerConfigBox::btnIdgamesURLDefaultClicked()
{
	leIdgamesURL->setText(Wadseeker::defaultIdgamesUrl());
}

void WadseekerConfigBox::btnUrlAddClicked()
{
	insertUrl(leUrl->text());
}

void WadseekerConfigBox::btnUrlDefaultClicked()
{
	for (int i = 0; !Wadseeker::defaultSites[i].isEmpty(); ++i)
	{
		this->insertUrl(Wadseeker::defaultSites[i]);
	}
}

void WadseekerConfigBox::btnUrlRemoveClicked()
{
	QItemSelectionModel* selModel = lstUrls->selectionModel();
	QModelIndexList indexList = selModel->selectedIndexes();
	selModel->clear();

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstUrls->model());
	QList<QStandardItem*> itemList;
	for (int i = 0; i < indexList.count(); ++i)
	{
		itemList << model->itemFromIndex(indexList[i]);
	}

	for (int i = 0; i < itemList.count(); ++i)
	{
		QModelIndex index = model->indexFromItem(itemList[i]);
		model->removeRow(index.row());
	}
}

ConfigurationBoxInfo* WadseekerConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->confBox = new WadseekerConfigBox(cfg, parent);
	ec->boxName = tr("Wadseeker");
	return ec;
}

void WadseekerConfigBox::focusChanged(QWidget* old, QWidget* now)
{
	if (now == leUrl)
	{
		emit wantChangeDefaultButton(btnUrlAdd);
	}
	else
	{
		emit wantChangeDefaultButton(NULL);
	}
}

void WadseekerConfigBox::insertUrl(const QUrl& url)
{
	if (url.isEmpty())
		return;

	// first we check whether the URL is already in the box.
	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstUrls->model());
	for (int i = 0; i < model->rowCount(); ++i)
	{
		QUrl existingUrl( model->item(i)->text() );
		if (existingUrl == url)
		{
			return;
		}
	}

	QStandardItem* it = new QStandardItem(url.toString());

	it->setDragEnabled(true);
	it->setDropEnabled(false);
	it->setToolTip(url.toString());

	model->appendRow(it);
}

void WadseekerConfigBox::readSettings()
{
	SettingsData* setting;

	setting = config->setting("WadPaths");
	cbTargetDirectory->clear();
	cbTargetDirectory->addItems(setting->string().split(";", QString::SkipEmptyParts));
	setting = config->setting("WadseekerTargetDirectory");
	cbTargetDirectory->setEditText(setting->string());

	setting = config->setting("WadseekerSearchURLs");
	QStringList urlLst = setting->string().split(";");
	QStringList::iterator it;
	for (it = urlLst.begin(); it != urlLst.end(); ++it)
	{
		this->insertUrl(QUrl::fromPercentEncoding(it->toAscii()));
	}

	setting = config->setting("WadseekerSearchInIdgames");
	bool b = static_cast<bool>(setting->integer());
	gboIdgamesArchive->setChecked(b);

	setting = config->setting("WadseekerIdgamesPriority");
	cboIdgamesPriority->setCurrentIndex(setting->integer());

	setting = config->setting("WadseekerIdgamesURL");
	leIdgamesURL->setText(setting->string());

	setting = config->setting("WadseekerConnectTimeoutSeconds");
	spinConnectTimeout->setValue(setting->integer());

	setting = config->setting("WadseekerDownloadTimeoutSeconds");
	spinDownloadTimeout->setValue(setting->integer());
}

void WadseekerConfigBox::saveSettings()
{
	SettingsData* setting;

	setting = config->setting("WadseekerTargetDirectory");
	setting->setValue(cbTargetDirectory->currentText());
	QFileInfo targetDirectoryInfo(cbTargetDirectory->currentText());
	if(!targetDirectoryInfo.isWritable())
		QMessageBox::warning(this, tr("Unwritable Target"), tr("The target directory you selected for Wadseeker can not be written to."));

	QStringList* urlLst = this->urlListEncoded();
	setting = config->setting("WadseekerSearchURLs");
	setting->setValue(urlLst->join(";"));

	setting = config->setting("WadseekerSearchInIdgames");
	setting->setValue(gboIdgamesArchive->isChecked());

	setting = config->setting("WadseekerIdgamesPriority");
	setting->setValue(cboIdgamesPriority->currentIndex());

	setting = config->setting("WadseekerIdgamesURL");
	setting->setValue(leIdgamesURL->text());

	setting = config->setting("WadseekerConnectTimeoutSeconds");
	setting->setValue(spinConnectTimeout->value());

	setting = config->setting("WadseekerDownloadTimeoutSeconds");
	setting->setValue(spinDownloadTimeout->value());

	delete urlLst;
}

QStringList* WadseekerConfigBox::urlListEncoded()
{
	QStringList* list = new QStringList();
	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstUrls->model());
	for (int i = 0; i < model->rowCount(); ++i)
	{
		QUrl existingUrl( model->item(i)->text() );
		(*list) << QUrl::toPercentEncoding(existingUrl.toString());
	}

	return list;
}
