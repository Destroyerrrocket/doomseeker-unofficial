//------------------------------------------------------------------------------
// cfgwadseekerappearance.h
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
#ifndef __WADSEEKERCONFIGAPPEARANCE_H__
#define __WADSEEKERCONFIGAPPEARANCE_H__

#include "gui/configuration/configpage.h"
#include "dptr.h"
#include <QIcon>

class CFGWadseekerAppearance : public ConfigPage
{
	Q_OBJECT

	public:
		CFGWadseekerAppearance(QWidget* parent = NULL);
		~CFGWadseekerAppearance();

		QIcon icon() const { return QIcon(":/icons/color-fill.png"); }
		QString name() const { return tr("Appearance"); }
		void readSettings();
		QString title() const { return tr("Wadseeker - Appearance"); }

	protected:
		void saveSettings();

	private:
		DPtr<CFGWadseekerAppearance> d;
};

#endif
