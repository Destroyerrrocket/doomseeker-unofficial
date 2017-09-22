//------------------------------------------------------------------------------
// cfgfilepaths.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "cfgfilepaths.h"
#include "ui_cfgfilepaths.h"
#include "configuration/doomseekerconfig.h"
#include "pathfinder/filesearchpath.h"
#include <QFileDialog>
#include <QStandardItem>

const int COL_PATH = 0;
const int COL_RECURSE = 1;

DClass<CFGFilePaths> : public Ui::CFGFilePaths
{
};

DPointered(CFGFilePaths)

CFGFilePaths::CFGFilePaths(QWidget* parent)
: ConfigPage(parent)
{
	d->setupUi(this);

	QStandardItemModel* model = new QStandardItemModel(this);
	d->lstIwadAndPwadPaths->setModel(model);

	QStringList labels;
	labels << tr("Path") << tr("Recurse");
	model->setHorizontalHeaderLabels(labels);

	QHeaderView* header = d->lstIwadAndPwadPaths->horizontalHeader();
#if QT_VERSION >= 0x050000
	header->setSectionResizeMode(COL_PATH, QHeaderView::Stretch);
	header->setSectionResizeMode(COL_RECURSE, QHeaderView::ResizeToContents);
#else
	header->setResizeMode(COL_PATH, QHeaderView::Stretch);
	header->setResizeMode(COL_RECURSE, QHeaderView::ResizeToContents);
#endif

	connect(d->btnAddWadPath, SIGNAL( clicked() ), this, SLOT( btnAddWadPath_Click()) );
	connect(d->btnRemoveWadPath, SIGNAL( clicked() ), this, SLOT( btnRemoveWadPath_Click()) );
	this->connect(d->lstIwadAndPwadPaths->itemDelegate(),
		SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)),
		SIGNAL(validationRequested()));
}

CFGFilePaths::~CFGFilePaths()
{
}

void CFGFilePaths::addPath(const FileSearchPath& fileSearchPath)
{
	if (fileSearchPath.isValid())
	{
		return;
	}

	QStandardItemModel *model = static_cast<QStandardItemModel*>(d->lstIwadAndPwadPaths->model());

	if (!isPathAlreadyDefined(fileSearchPath.path()))
	{
		QStandardItem *path = new QStandardItem(fileSearchPath.path());
		path->setData(fileSearchPath.path(), Qt::ToolTipRole);
		QStandardItem *recurse = new QStandardItem();
		recurse->setCheckable(true);
		recurse->setCheckState(fileSearchPath.isRecursive() ? Qt::Checked : Qt::Unchecked);
		recurse->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
		QList<QStandardItem*> items;
		items << path;
		items << recurse;
		model->appendRow(items);
		d->lstIwadAndPwadPaths->resizeRowsToContents();
	}
}

void CFGFilePaths::btnAddWadPath_Click()
{
	QString strDir = QFileDialog::getExistingDirectory(this, tr("Doomseeker - Add wad path"));
	addPath(strDir);
	emit validationRequested();
}

void CFGFilePaths::btnRemoveWadPath_Click()
{
	QItemSelectionModel* selModel = d->lstIwadAndPwadPaths->selectionModel();
	QModelIndexList indexList = selModel->selectedRows();
	selModel->clear();

	QStandardItemModel* model = static_cast<QStandardItemModel*>(d->lstIwadAndPwadPaths->model());
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
	emit validationRequested();
}

QIcon CFGFilePaths::icon() const
{
	return QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon);
}

bool CFGFilePaths::isPathAlreadyDefined(const QString& path)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(d->lstIwadAndPwadPaths->model());

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

	d->cbTellMeWhereAreMyWads->setChecked(gConfig.doomseeker.bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn);
}

void CFGFilePaths::saveSettings()
{
	QList<FileSearchPath> wadPaths;

	QStandardItemModel* model = static_cast<QStandardItemModel*>(d->lstIwadAndPwadPaths->model());
	for(int i = 0; i < model->rowCount(); ++i)
	{
		QStandardItem* itemPath = model->item(i, COL_PATH);
		QStandardItem* itemRecurse = model->item(i, COL_RECURSE);
		FileSearchPath fileSearchPath(itemPath->text());
		fileSearchPath.setRecursive(itemRecurse->checkState() == Qt::Checked);
		wadPaths << fileSearchPath;
	}

	gConfig.doomseeker.wadPaths = wadPaths;
	gConfig.doomseeker.bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn = d->cbTellMeWhereAreMyWads->isChecked();
}

ConfigPage::Validation CFGFilePaths::validate()
{
	bool allPathsValid = true;
	QStandardItemModel *model = static_cast<QStandardItemModel*>(d->lstIwadAndPwadPaths->model());
	for (int i = 0; i < model->rowCount(); ++i)
	{
		QStandardItem *itemPath = model->item(i, COL_PATH);

		QString validationError = validatePath(itemPath->text());
		bool valid = validationError.isEmpty();
		allPathsValid = allPathsValid && valid;

		itemPath->setIcon(valid ? QIcon() : QIcon(":/icons/exclamation_16.png"));
		itemPath->setToolTip(validationError);
	}
	return allPathsValid ? VALIDATION_OK : VALIDATION_ERROR;
}

QString CFGFilePaths::validatePath(const QString &path) const
{
	if (path.trimmed().isEmpty())
	{
		return tr("No path specified.");
	}

	QFileInfo fileInfo(path.trimmed());
	if (!fileInfo.exists())
	{
		return tr("Path doesn't exist.");
	}

	if (!fileInfo.isDir())
	{
		return tr("Path is not a directory.");
	}

	return QString();
}
