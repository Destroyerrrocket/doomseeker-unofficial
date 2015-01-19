//------------------------------------------------------------------------------
// cfgwadseekeridgames.h
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
#ifndef __WADSEEKERCONFIG_IDGAMES_H_
#define __WADSEEKERCONFIG_IDGAMES_H_

#include "gui/configuration/configurationbasebox.h"
#include <QIcon>

class CFGWadseekerIdgames : public ConfigurationBaseBox
{
	Q_OBJECT

	public:
		CFGWadseekerIdgames(QWidget* parent = NULL);
		~CFGWadseekerIdgames();

		QIcon icon() const { return QIcon(":/icons/arrow-down-double.png"); }
		QString name() const { return tr("Idgames"); }
		void readSettings();
		QString title() const { return tr("Wadseeker - Idgames"); }

	protected slots:
		void btnIdgamesURLDefaultClicked();
		void cbUseIdgamesToggled(bool checked);

	protected:
		void saveSettings();

	private:
		class PrivData;
		PrivData *d;
};

#endif
