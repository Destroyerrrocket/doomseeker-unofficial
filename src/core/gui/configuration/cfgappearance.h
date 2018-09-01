//------------------------------------------------------------------------------
// cfgappearance.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#ifndef __CFG_APPEARANCE_H__
#define __CFG_APPEARANCE_H__

#include "gui/configuration/configpage.h"
#include "dptr.h"
#include <QIcon>

class CFGAppearance : public ConfigPage
{
	Q_OBJECT

	public:
		CFGAppearance(QWidget *parent=NULL);
		~CFGAppearance();

		QIcon icon() const { return QIcon(":/icons/color-fill.png"); }
		QString name() const { return tr("Appearance"); }

		void readSettings();

	protected:
		void saveSettings();

	private:
		void initLanguagesList();
		void initSlotStyles(const QString &selected);
		void reject_();
		void saveDynamicSettings();

		DPtr<CFGAppearance> d;

	private slots:
		void dynamicAppearanceChange();
};

#endif /* __CFG_APPEARANCE_H__ */
