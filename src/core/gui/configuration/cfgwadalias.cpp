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

#include "configuration/doomseekerconfig.h"
#include "gui/commongui.h"
#include "pathfinder/filealias.h"

class CFGWadAlias::PrivData
{
public:
	enum Columns
	{
		ColWad,
		ColAliases
	};
};

CFGWadAlias::CFGWadAlias(QWidget *parent)
: ConfigurationBaseBox(parent)
{
	setupUi(this);
	d = new PrivData();
	QHeaderView *header = table->horizontalHeader();
	header->resizeSection(PrivData::ColWad, 150);
	header->setResizeMode(PrivData::ColAliases, QHeaderView::Stretch);
	table->sortByColumn(PrivData::ColWad, Qt::AscendingOrder);
}

CFGWadAlias::~CFGWadAlias()
{
	delete d;
}

void CFGWadAlias::addAliasToTable(const FileAlias &alias)
{
	bool wasSortingEnabled = table->isSortingEnabled();
	table->setSortingEnabled(false);

	int row = findRowWithWad(alias.name());
	if (row < 0)
	{
		row = table->rowCount();
		table->insertRow(row);
		applyAliasToRow(row, alias);
	}
	else
	{
		// Merge.
		FileAlias existingAlias = aliasFromRow(row);
		existingAlias.addAliases(alias.aliases());
		applyAliasToRow(row, existingAlias);
	}

	table->setSortingEnabled(wasSortingEnabled);
	table->resizeRowsToContents();
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
	bool wasSortingEnabled = table->isSortingEnabled();
	table->setSortingEnabled(false);

	int row = table->rowCount();
	table->insertRow(row);
	table->setItem(row, PrivData::ColWad, new QTableWidgetItem());
	table->setItem(row, PrivData::ColAliases, new QTableWidgetItem());

	table->setSortingEnabled(wasSortingEnabled);
}

QList<FileAlias> CFGWadAlias::aliases() const
{
	QList<FileAlias> aliases;
	for (int row = 0; row < table->rowCount(); ++row)
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
	alias.setName(table->item(row, PrivData::ColWad)->text().trimmed());
	QStringList candidateAliases = table->item(row, PrivData::ColAliases)->text().split(";");
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
	table->setItem(row, PrivData::ColWad, toolTipItem(alias.name()));
	table->setItem(row, PrivData::ColAliases, toolTipItem(alias.aliases().join("; ")));
}

int CFGWadAlias::findRowWithWad(const QString &wadName)
{
	for (int row = 0; row < table->rowCount(); ++row)
	{
		if (table->item(row, PrivData::ColWad)->text().trimmed().compare(
			wadName.trimmed(), Qt::CaseInsensitive) == 0)
		{
			return row;
		}
	}
	return -1;
}

void CFGWadAlias::readSettings()
{
	while (table->rowCount() > 0)
	{
		table->removeRow(0);
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
	CommonGUI::removeSelectedRowsFromQTableWidget(table);
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
