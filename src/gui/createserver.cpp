//------------------------------------------------------------------------------
// createserver.cpp
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
#include "createserver.h"
#include "main.h"

#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QObject>

CreateServerDlg::CreateServerDlg(QWidget* parent) : QDialog(parent)
{
	currentEngine = NULL;

	setupUi(this);
	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT ( btnClicked(QAbstractButton *) ));
	connect(btnAddPwad, SIGNAL( clicked() ), this, SLOT ( btnAddPwadClicked() ) );
	connect(btnIwadBrowse, SIGNAL( clicked() ), this, SLOT ( btnIwadBrowseClicked() ) );
	connect(btnRemovePwad, SIGNAL( clicked() ), this, SLOT ( btnRemovePwadClicked() ) );

	connect(cboEngine, SIGNAL( currentIndexChanged(int) ), this, SLOT( cboEngineSelected(int) ) );

	cboIwad->setEditable(true);
	lstAdditionalFiles->setModel(new QStandardItemModel(this));

	initPrimary();
}

CreateServerDlg::~CreateServerDlg()
{

}

void CreateServerDlg::addWadPath(const QString& strPath)
{
	if (strPath.isEmpty())
	{
		return;
	}

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstAdditionalFiles->model());

	// Check if this path exists already, if so - do nothing.
	for(int i = 0; i < model->rowCount(); ++i)
	{
		QStandardItem* item = model->item(i);
		QString dir = item->text();
		Qt::CaseSensitivity cs;

		#ifdef Q_WS_WIN
		cs = Qt::CaseInsensitive;
		#else
		cs = Qt::CaseSensitive;
		#endif

		if (dir.compare(strPath, cs) == 0)
		{
			return;
		}
	}

	QStandardItem* it = new QStandardItem(strPath);

	it->setDragEnabled(true);
	it->setDropEnabled(false);
	it->setToolTip(strPath);

	model->appendRow(it);
}

void CreateServerDlg::btnAddPwadClicked()
{
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - Add file"));
	addWadPath(strFile);
}

void CreateServerDlg::btnClicked(QAbstractButton *button)
{
	// Figure out what button we pressed and perform its action.
	switch(buttonBox->standardButton(button))
	{
		default:
			break;
		case QDialogButtonBox::Ok:
			this->accept();
			break;

		case QDialogButtonBox::Cancel:
			this->reject();
			break;
	}
}

void CreateServerDlg::btnIwadBrowseClicked()
{
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - select IWAD"));
	if (!strFile.isEmpty())
	{
		cboIwad->addItem(strFile);
		cboIwad->setCurrentIndex(cboIwad->count() - 1);
	}
}

void CreateServerDlg::btnRemovePwadClicked()
{
	QItemSelectionModel* selModel = lstAdditionalFiles->selectionModel();
	QModelIndexList indexList = selModel->selectedIndexes();
	selModel->clear();

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstAdditionalFiles->model());
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

void CreateServerDlg::cboEngineSelected(int index)
{
	if (index >= 0)
	{
		int enginePluginIndex = cboEngine->itemData(index).toInt();
		if (enginePluginIndex < Main::enginePlugins.numPlugins())
		{
			const PluginInfo* nfo = Main::enginePlugins[enginePluginIndex]->info;

			initEngineSpecific(nfo);
		}
	}
}

void CreateServerDlg::initEngineSpecific(const PluginInfo* engineInfo)
{
	if (engineInfo == currentEngine || engineInfo == NULL)
		return;

	currentEngine = engineInfo;
	spinPort->setValue(engineInfo->pInterface->defaultServerPort());

	cboGamemode->clear();

	const GameMode* gameModes = engineInfo->pInterface->gameModes();
	for (int i = 0; i < engineInfo->pInterface->gameModesNumber(); ++i)
	{
		cboGamemode->addItem(gameModes[i].name(), i);
	}
}

void CreateServerDlg::initPrimary()
{
	cboEngine->clear();

	for (int i = 0; i < Main::enginePlugins.numPlugins(); ++i)
	{
		const PluginInfo* nfo = Main::enginePlugins[i]->info;
		cboEngine->addItem(nfo->pInterface->icon(), nfo->name, i);
	}

	if (cboEngine->count() > 0)
	{
		cboEngine->setCurrentIndex(0);
	}

	cboDifficulty->clear();

	cboDifficulty->addItem("1 - I'm too young to die", 0);
	cboDifficulty->addItem("2 - Hey, not too rough", 1);
	cboDifficulty->addItem("3 - Hurt me plenty", 2);
	cboDifficulty->addItem("4 - Ultra-violence", 3);
	cboDifficulty->addItem("5 - NIGHTMARE!", 4);

	const QString iwads[] = { "doom.wad", "doom1.wad", "doom2.wad", "tnt.wad", "plutonia.wad", "hexen.wad", "hexdd.wad", "freedoom.wad", "strife1.wad", "" };

	cboIwad->clear();

	for (int i = 0; !iwads[i].isEmpty(); ++i)
	{
		PathFinder pf(Main::config);
		QString path = pf.findWad(iwads[i]);
		if (!path.isEmpty())
			cboIwad->addItem(path);
	}
}
