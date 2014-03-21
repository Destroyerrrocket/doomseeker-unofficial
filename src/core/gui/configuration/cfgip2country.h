//------------------------------------------------------------------------------
// cfgip2country.h
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

#ifndef __CFG_IP2COUNTRY_H__
#define __CFG_IP2COUNTRY_H__

#include "gui/configuration/configurationbasebox.h"
#include "ui_cfgip2country.h"

class CFGIP2Country : public ConfigurationBaseBox, private Ui::CFGIP2Country
{
	Q_OBJECT

	public:
		CFGIP2Country(QWidget *parent=NULL);
		
		QIcon icon() const { return QIcon(":/icons/emblem-web.png"); }
		QString name() const { return tr("IP2C"); }
		void readSettings();

	protected:
		void saveSettings();
};

#endif
