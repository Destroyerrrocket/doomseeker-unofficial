//------------------------------------------------------------------------------
// wadspicker.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "wadspicker.h"
#include "ui_wadspicker.h"

#include "configuration/doomseekerconfig.h"
#include "gui/commongui.h"
#include <QFileDialog>
#include <QStandardItemModel>

class WadsPicker::PrivData : public Ui::WadsPicker
{
};

WadsPicker::WadsPicker(QWidget *parent)
{
	d = new PrivData;
	d->setupUi(this);
	d->lstAdditionalFiles->setModel(new QStandardItemModel(this));
}

WadsPicker::~WadsPicker()
{
	delete d;
}

void WadsPicker::addWadPath(const QString &wadPath)
{
	if (wadPath.isEmpty())
	{
		return;
	}
	QFileInfo fileInfo(wadPath);
	if (!fileInfo.isFile())
	{
		return;
	}

	QStandardItemModel* model = static_cast<QStandardItemModel*>(d->lstAdditionalFiles->model());

	// Check if this path exists already, if so - do nothing.
	for(int i = 0; i < model->rowCount(); ++i)
	{
		QStandardItem* item = model->item(i);
		QString dir = item->text();
		Qt::CaseSensitivity cs;

		#ifdef Q_OS_WIN32
		cs = Qt::CaseInsensitive;
		#else
		cs = Qt::CaseSensitive;
		#endif

		if (dir.compare(wadPath, cs) == 0)
		{
			return;
		}
	}

	QStandardItem* it = new QStandardItem(wadPath);

	it->setDragEnabled(true);
	it->setDropEnabled(false);
	it->setToolTip(wadPath);

	model->appendRow(it);
}

void WadsPicker::browseAndAdd()
{
	QString dialogDir = gConfig.doomseeker.previousCreateServerWadDir;
	QStringList filesNames = QFileDialog::getOpenFileNames(this,
		tr("Doomseeker - Add file(s)"), dialogDir);

	if (!filesNames.isEmpty())
	{
		// Remember the directory of the first file. This directory will be
		// restored the next time this dialog is opened.
		QFileInfo fi(filesNames[0]);
		gConfig.doomseeker.previousCreateServerWadDir = fi.absolutePath();

		foreach (const QString& strFile, filesNames)
		{
			addWadPath(strFile);
		}
	}
}

QStringList WadsPicker::filePaths() const
{
	return CommonGUI::listViewStandardItemsToStringList(d->lstAdditionalFiles);
}

void WadsPicker::setFilePaths(const QStringList &paths)
{
	removeAll();
	foreach (const QString &path, paths)
	{
		addWadPath(path);
	}
}

void WadsPicker::removeAll()
{
	QStandardItemModel* pModel = (QStandardItemModel*)d->lstAdditionalFiles->model();
	pModel->clear();
}

void WadsPicker::removeSelected()
{
	const bool bSelectNextLowest = true;
	CommonGUI::removeSelectedRowsFromStandardItemView(d->lstAdditionalFiles, bSelectNextLowest);
}
