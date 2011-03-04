//------------------------------------------------------------------------------
// cfgquery.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __CFG_QUERY_H__
#define __CFG_QUERY_H__

#include "gui/configuration/configurationbasebox.h"
#include "ui_cfgquery.h"

class CFGQuery : public ConfigurationBaseBox, private Ui::CFGQuery
{
	Q_OBJECT

	public:
		CFGQuery(QWidget *parent=NULL);

		QIcon							icon() const { return QIcon(":/icons/view-refresh-2.png"); }
		QString							name() const { return tr("Query"); }
		void							readSettings();

	protected:
		void							saveSettings();
};

#endif /* __CFG_QUERY_H__ */
