//------------------------------------------------------------------------------
// flagspagevaluecontroller.h
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef DOOMSEEKER_PLUGIN_ZANDRONUM_CREATESERVERDIALOGPAGES_FLAGSPAGEVALUECONTROLLER_H
#define DOOMSEEKER_PLUGIN_ZANDRONUM_CREATESERVERDIALOGPAGES_FLAGSPAGEVALUECONTROLLER_H

#include <QCheckBox>
#include <QMap>

class FlagsPage;

/**
 * @brief Converts numerical flags values to widget representation
 *        and vice-versa.
 */
class FlagsPageValueController
{
	public:
		FlagsPageValueController(FlagsPage* flagsPage);

		void convertNumericalToWidgets();
		void convertWidgetsToNumerical();

	private:
		unsigned compatflags;
		unsigned compatflags2;

		unsigned dmflags;
		unsigned dmflags2;
		unsigned dmflags3;

		QMap<unsigned, QCheckBox*> compatflagsCheckboxes;
		QMap<unsigned, QCheckBox*> compatflags2Checkboxes;
		QMap<unsigned, QCheckBox*> dmflagsCheckboxes;
		QMap<unsigned, QCheckBox*> dmflags2Checkboxes;
		QMap<unsigned, QCheckBox*> dmflags3Checkboxes;

		FlagsPage* flagsPage;

		void convertToNumericalGeneral();
		void convertToNumericalPlayers();
		void convertToNumericalCooperative();

		void convertCheckboxesToNumerical(const QMap<unsigned, QCheckBox*>& checkboxMap,
			unsigned& flagsValue);
		void convertNumericalToCheckboxes(QMap<unsigned, QCheckBox*>& checkboxMap,
			unsigned flagsValue);

		void convertToWidgetGeneral();
		void convertToWidgetPlayers();
		void convertToWidgetCooperative();
};

#endif
