//------------------------------------------------------------------------------
// cfgfilepaths.cpp
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
#include "cfgfilepaths.h"
#include "configuration/doomseekerconfig.h"
#include <QFileDialog>
#include <QStandardItem>

CFGFilePaths::CFGFilePaths(QWidget* parent) 
: ConfigurationBaseBox(parent)
{
	setupUi(this);

	QAbstractItemModel* model = new QStandardItemModel();
	lstIwadAndPwadPaths->setModel(model);

	connect(btnAddWadPath, SIGNAL( clicked() ), this, SLOT( btnAddWadPath_Click()) );
	connect(btnRemoveWadPath, SIGNAL( clicked() ), this, SLOT( btnRemoveWadPath_Click()) );
}

void CFGFilePaths::addPath(const QString& strPath)
{
	if (strPath.isEmpty())
	{
		return;
	}

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstIwadAndPwadPaths->model());

	if (!isPathAlreadyDefined(strPath))
	{
		model->appendRow(new QStandardItem(strPath));
	}
}

void CFGFilePaths::btnAddWadPath_Click()
{
	QString strDir = QFileDialog::getExistingDirectory(this, tr("Doomseeker - Add wad path"));
	addPath(strDir);
}

void CFGFilePaths::btnRemoveWadPath_Click()
{
	QItemSelectionModel* selModel = lstIwadAndPwadPaths->selectionModel();
	QModelIndexList indexList = selModel->selectedIndexes();
	selModel->clear();

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstIwadAndPwadPaths->model());
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

bool CFGFilePaths::isPathAlreadyDefined(const QString& path)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstIwadAndPwadPaths->model());

	Qt::CaseSensitivity caseSensitivity;

	#ifdef Q_OS_WIN32
	caseSensitivity = Qt::CaseInsensitive;
	#else
	caseSensitivity = Qt::CaseSensitive;
	#endif

	for(int i = 0; i < model->rowCount(); ++i)
	{
		QStandardItem* item = model->item(i);
		QString dir = item->text();

		if (dir.compare(path, caseSensitivity) == 0)
		{
			return true;
		}
	}

	return false;
}

void CFGFilePaths::readSettings()
{
	const QStringList& wadPaths = gConfig.doomseeker.wadPaths;
	for (int i = 0; i < wadPaths.count(); ++i)
	{
		addPath(wadPaths[i]);
	}

	cbTellMeWhereAreMyWads->setChecked(gConfig.doomseeker.bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn);
}

void CFGFilePaths::saveSettings()
{
	QStringList wadPaths;

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstIwadAndPwadPaths->model());
	{
		for(int i = 0; i < model->rowCount(); ++i)
		{
			QStandardItem* item = model->item(i);
			wadPaths << item->text();
		}
	}

	gConfig.doomseeker.wadPaths = wadPaths;
	gConfig.doomseeker.bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn = cbTellMeWhereAreMyWads->isChecked();
}
