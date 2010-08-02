//------------------------------------------------------------------------------
// cfgip2c.h
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

#ifndef __CFG_IP2C_H__
#define __CFG_IP2C_H__

#include "gui/configBase.h"
#include "ui_cfgip2c.h"

class IP2CConfigBox : public ConfigurationBaseBox, private Ui::IP2CConfigBox
{
	Q_OBJECT

	public:
		static ConfigurationBoxInfo	*createStructure(IniSection *cfg, QWidget *parent=NULL);

		void	readSettings();

	protected:
		IP2CConfigBox(IniSection *cfg, QWidget *parent=NULL);

		void	saveSettings();
};

#endif /* __CFG_QUERY_H__ */
