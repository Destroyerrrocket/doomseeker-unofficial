//------------------------------------------------------------------------------
// cfgfilepaths.h
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

#ifndef __CFG_FILE_PATHS_H_
#define __CFG_FILE_PATHS_H_

#include "gui/configuration/configurationbasebox.h"
#include "ui_cfgfilepaths.h"

class FileSearchPath;

class CFGFilePaths : public ConfigurationBaseBox, private Ui::CFGFilePaths
{
	Q_OBJECT

	public:
		CFGFilePaths(QWidget* parent = 0);

		void 							readSettings();

	protected:
		void 							addPath(const FileSearchPath& fileSearchPath);

		QIcon							icon() const
		{
			return QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon);
		}

		bool							isPathAlreadyDefined(const QString& path);

		QString							name() const
		{
			return tr("File Paths");
		}

		void 							saveSettings();

	protected slots:
		void 							btnAddWadPath_Click();
		void 							btnRemoveWadPath_Click();

};

#endif
