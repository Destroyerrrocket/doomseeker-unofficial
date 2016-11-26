//------------------------------------------------------------------------------
// cfgwadalias.h
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
#ifndef idBE57B8CD_234F_4EEE_AC1B1FAAFB11B1CD
#define idBE57B8CD_234F_4EEE_AC1B1FAAFB11B1CD

#include "gui/configuration/configurationbasebox.h"
#include "dptr.h"

class FileAlias;
class QTableWidgetItem;

class CFGWadAlias : public ConfigurationBaseBox
{
	Q_OBJECT

	public:
		CFGWadAlias(QWidget *parent=NULL);
		~CFGWadAlias();

		QIcon icon() const;
		QString name() const { return tr("WAD Aliases"); }

		void readSettings();

	protected:
		void saveSettings();

	private:
		DPtr<CFGWadAlias> d;

		void addAliasToTable(const FileAlias &alias);
		FileAlias aliasFromRow(int row) const;
		void applyAliasToRow(int row, const FileAlias &alias);
		QList<FileAlias> aliases() const;
		int findRowWithWad(const QString &wadName);
		void resizeRowsToContents();
		QTableWidgetItem *toolTipItem(const QString &contents);

	private slots:
		void addDefaults();
		void addNewEntry();
		void removeSelected();
};

#endif
