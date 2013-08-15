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
#include "pathfinder/filesearchpath.h"
#include <QFileDialog>
#include <QStandardItem>

const int COL_PATH = 0;
const int COL_RECURSE = 1;

CFGFilePaths::CFGFilePaths(QWidget* parent) 
: ConfigurationBaseBox(parent)
{
	setupUi(this);

	QStandardItemModel* model = new QStandardItemModel(this);
	lstIwadAndPwadPaths->setModel(model);

	QStringList labels;
	labels << tr("Path") << tr("Recurse");
	model->setHorizontalHeaderLabels(labels);

	QHeaderView* header = lstIwadAndPwadPaths->horizontalHeader();
	header->setResizeMode(COL_PATH, QHeaderView::Stretch);
	header->setResizeMode(COL_RECURSE, QHeaderView::ResizeToContents);

	connect(btnAddWadPath, SIGNAL( clicked() ), this, SLOT( btnAddWadPath_Click()) );
	connect(btnRemoveWadPath, SIGNAL( clicked() ), this, SLOT( btnRemoveWadPath_Click()) );
}

void CFGFilePaths::addPath(const FileSearchPath& fileSearchPath)
{
	if (fileSearchPath.isValid())
	{
		return;
	}

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstIwadAndPwadPaths->model());

	if (!isPathAlreadyDefined(fileSearchPath.path()))
	{
		QStandardItem* path = new QStandardItem(fileSearchPath.path());
		path->setData(fileSearchPath.path(), Qt::ToolTipRole);
		QStandardItem* recurse = new QStandardItem();
		recurse->setCheckable(true);
		recurse->setCheckState(fileSearchPath.isRecursive() ? Qt::Checked : Qt::Unchecked);
		recurse->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
		QList<QStandardItem*> items;
		items << path;
		items << recurse;
		model->appendRow(items);
		lstIwadAndPwadPaths->resizeRowsToContents();
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
	QModelIndexList indexList = selModel->selectedRows();
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
	const QList<FileSearchPath>& wadPaths = gConfig.doomseeker.wadPaths;
	for (int i = 0; i < wadPaths.count(); ++i)
	{
		addPath(wadPaths[i]);
	}

	cbTellMeWhereAreMyWads->setChecked(gConfig.doomseeker.bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn);
}

void CFGFilePaths::saveSettings()
{
	QList<FileSearchPath> wadPaths;

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstIwadAndPwadPaths->model());
	{
		for(int i = 0; i < model->rowCount(); ++i)
		{
			QStandardItem* itemPath = model->item(i, COL_PATH);
			QStandardItem* itemRecurse = model->item(i, COL_RECURSE);
			FileSearchPath fileSearchPath(itemPath->text());
			fileSearchPath.setRecursive(itemRecurse->checkState() == Qt::Checked);
			wadPaths << fileSearchPath;
		}
	}

	gConfig.doomseeker.wadPaths = wadPaths;
	gConfig.doomseeker.bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn = cbTellMeWhereAreMyWads->isChecked();
}
