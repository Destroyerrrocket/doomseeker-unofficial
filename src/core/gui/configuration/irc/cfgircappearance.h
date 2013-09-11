//------------------------------------------------------------------------------
// cfgircappearance.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __CFGIRCAPPEARANCE_H__
#define __CFGIRCAPPEARANCE_H__

#include "ui_cfgircappearance.h"
#include "gui/configuration/configurationbasebox.h"

class CFGIRCAppearance : public ConfigurationBaseBox, private Ui::CFGIRCAppearance 
{
	Q_OBJECT

	public:
		CFGIRCAppearance(QWidget* parent = NULL);
		
		QIcon							icon() const { return QIcon(":/icons/color-fill.png"); }
		QString							name() const { return tr("Appearance"); }
		void							readSettings();
		
	protected:
		void							saveSettings();		
};

#endif
