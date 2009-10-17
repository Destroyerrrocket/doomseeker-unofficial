//------------------------------------------------------------------------------
// engineChocolateDoomConfig.h
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

#ifndef __ENGINE_CHOCOLATEDOOM_CONFIG_H_
#define __ENGINE_CHOCOLATEDOOM_CONFIG_H_

#include "gui/configBase.h"
#include "ui_engineChocolateDoomConfig.h"

class EngineChocolateDoomConfigBox : public ConfigurationBaseBox, private Ui::EngineChocolateDoomConfigBox
{
	Q_OBJECT

	public:
		static ConfigurationBoxInfo* createStructure(Config* cfg, QWidget* parent = NULL);

		void readSettings();


	protected slots:
		void btnBrowseClientBinaryClicked();
		void btnBrowseServerBinaryClicked();

	protected:
		EngineChocolateDoomConfigBox(Config* cfg, QWidget* parent = NULL);
		void saveSettings();
};

#endif