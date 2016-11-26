//------------------------------------------------------------------------------
// cfgwadalias.cpp
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
#include "cfgwadalias.h"
#include "ui_cfgwadalias.h"

#include "configuration/doomseekerconfig.h"
#include "gui/commongui.h"
#include "pathfinder/filealias.h"
#include <QTimer>

DClass<CFGWadAlias> : public Ui::CFGWadAlias
{
public:
	enum Columns
	{
		ColWad,
		ColAliases
	};

	QTimer resizeTimer;
};

DPointeredNoCopy(CFGWadAlias)

CFGWadAlias::CFGWadAlias(QWidget *parent)
: ConfigurationBaseBox(parent)
{
	d->setupUi(this);

	connect(&d->resizeTimer, SIGNAL(timeout()), d->table, SLOT(resizeRowsToContents()));

	QHeaderView *header = d->table->horizontalHeader();
	header->resizeSection(PrivData<CFGWadAlias>::ColWad, 150);
#if QT_VERSION >= 0x050000
	header->setSectionResizeMode(PrivData<CFGWadAlias>::ColAliases, QHeaderView::Stretch);
#else
	header->setResizeMode(PrivData<CFGWadAlias>::ColAliases, QHeaderView::Stretch);
#endif
	d->table->sortByColumn(PrivData<CFGWadAlias>::ColWad, Qt::AscendingOrder);
}

CFGWadAlias::~CFGWadAlias()
{
}

void CFGWadAlias::addAliasToTable(const FileAlias &alias)
{
	bool wasSortingEnabled = d->table->isSortingEnabled();
	d->table->setSortingEnabled(false);

	int row = findRowWithWad(alias.name());
	if (row < 0)
	{
		row = d->table->rowCount();
		d->table->insertRow(row);
		applyAliasToRow(row, alias);
	}
	else
	{
		// Merge.
		FileAlias existingAlias = aliasFromRow(row);
		existingAlias.addAliases(alias.aliases());
		applyAliasToRow(row, existingAlias);
	}

	d->table->setSortingEnabled(wasSortingEnabled);
	resizeRowsToContents();
}

void CFGWadAlias::addDefaults()
{
	QList<FileAlias> aliases = FileAlias::standardWadAliases();
	foreach (const FileAlias& alias, aliases)
	{
		addAliasToTable(alias);
	}
}

void CFGWadAlias::addNewEntry()
{
	bool wasSortingEnabled = d->table->isSortingEnabled();
	d->table->setSortingEnabled(false);

	int row = d->table->rowCount();
	d->table->insertRow(row);
	d->table->setItem(row, PrivData<CFGWadAlias>::ColWad, new QTableWidgetItem());
	d->table->setItem(row, PrivData<CFGWadAlias>::ColAliases, new QTableWidgetItem());

	d->table->setSortingEnabled(wasSortingEnabled);
}

QList<FileAlias> CFGWadAlias::aliases() const
{
	QList<FileAlias> aliases;
	for (int row = 0; row < d->table->rowCount(); ++row)
	{
		FileAlias alias = aliasFromRow(row);
		if (alias.isValid())
		{
			aliases << alias;
		}
	}
	return FileAliasList::mergeDuplicates(aliases);
}

FileAlias CFGWadAlias::aliasFromRow(int row) const
{
	FileAlias alias;
	alias.setName(d->table->item(row, PrivData<CFGWadAlias>::ColWad)->text().trimmed());
	QStringList candidateAliases = d->table->item(row, PrivData<CFGWadAlias>::ColAliases)->text().split(";");
	foreach (const QString &candidateAlias, candidateAliases)
	{
		if (!candidateAlias.trimmed().isEmpty())
		{
			alias.addAlias(candidateAlias.trimmed());
		}
	}
	return alias;
}

void CFGWadAlias::applyAliasToRow(int row, const FileAlias &alias)
{
	d->table->setItem(row, PrivData<CFGWadAlias>::ColWad, toolTipItem(alias.name()));
	d->table->setItem(row, PrivData<CFGWadAlias>::ColAliases, toolTipItem(alias.aliases().join("; ")));
}

int CFGWadAlias::findRowWithWad(const QString &wadName)
{
	for (int row = 0; row < d->table->rowCount(); ++row)
	{
		if (d->table->item(row, PrivData<CFGWadAlias>::ColWad)->text().trimmed().compare(
			wadName.trimmed(), Qt::CaseInsensitive) == 0)
		{
			return row;
		}
	}
	return -1;
}

QIcon CFGWadAlias::icon() const
{
	return QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon);
}

void CFGWadAlias::readSettings()
{
	while (d->table->rowCount() > 0)
	{
		d->table->removeRow(0);
	}
	// Aliases from configuration are guaranteed to be unique.
	QList<FileAlias> aliases = gConfig.doomseeker.wadAliases();
	foreach (const FileAlias &alias, aliases)
	{
		if (alias.isValid())
		{
			addAliasToTable(alias);
		}
	}
}

void CFGWadAlias::removeSelected()
{
	CommonGUI::removeSelectedRowsFromQTableWidget(d->table);
}

void CFGWadAlias::resizeRowsToContents()
{
	d->resizeTimer.start(0);
}

void CFGWadAlias::saveSettings()
{
	gConfig.doomseeker.setWadAliases(aliases());
}

QTableWidgetItem *CFGWadAlias::toolTipItem(const QString &contents)
{
	QTableWidgetItem *item = new QTableWidgetItem(contents);
	item->setToolTip(contents);
	return item;
}
